#ifndef SERVER_THREAD
#define SERVER_THREAD
#include "./server_head.h"
#include "./server_class.h"
// 等待连接线程
void AccpetThread(Broke* const);
// 处理/管理 新连接
void ConnectThread(Broke* const, int, sockaddr_in);
// 新节点的 读/写 线程
void ReadThread(Broke* const, shared_ptr<Node> mynode);
void WriteThread(Broke* const, shared_ptr<Node> mynode);
// broke进行数据转发
void MsgCopyToNode(Broke* const, Msg, string, uint32_t);
/* ---------------------------------------------------------------------- */
void AccpetThread(Broke* const b) {
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t len;
    len = sizeof(cliaddr);
    printf("Wait client...\n");
    while (1) {
        clifd = b->WaitAccpet(cliaddr);
        if (clifd == -1)
            perror("accpet error:");
        else
        {
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
            // 新的客户端连接
            thread t(ConnectThread, b, clifd, cliaddr);
            t.detach();
        }
    }
}
/* ---------------------------------------------------------------------- */
void ConnectThread(Broke* const b, int connectfd, struct sockaddr_in cliaddr) {
    shared_ptr<Node> mynode(new Node(inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port, connectfd));
    int32_t index_temp = b->AllocIndex();
    if (index_temp <= 0 || mynode->SetIndex(index_temp) <= 0) {
        // 序号分配/设置出错
        mynode->SetState(_connect_wait);
    }
    else {
        mynode->SetState(_connecting);
    }
    switch (mynode->GetState())
    {
        case _connect_wait:
        {
            // 此线程一直尝试 获取序号/分配序号
            thread t([b, mynode](){
                int32_t index_temp = 0x00000000;
                while (index_temp <= 0 || mynode->SetIndex(index_temp) == 0 )
                    index_temp = b->AllocIndex();
            });
            t.detach();
            // 调用重设置函数，一直阻塞在这里，默认等待时间 3 秒
            if (mynode->ResetIndex(3)) {
                mynode->SetState(_connecting);
            }
            else {
                mynode->SetState(_close);
                break;
            }
        }
        case _connecting:
        {
            // 添加 mynode 进入 broke 的管理
            // 加入 broke 成功
            if (b->AddNode(mynode)) {
                // 设置状态
                mynode->SetState(_connected);
                // 开启 读写 线程
                mynode->ProtectThread.lock();
                thread t1(ReadThread, b, mynode);
                t1.detach();
                mynode->ProtectThread.unlock();
                thread t2(WriteThread, b, mynode);
                t2.detach();
            }
            else {
            // 加入 broke 失败
                // 设置状态
                mynode->SetState(_close);
            }
            break;
        }
        default:
        {
            printf("something error happened in [ConnectThread, switch]\n");
            mynode->SetState(_close);
            break;
        }
    }
    mynode->WaitForClose();
    // broke 删除 mynode
    b->DelNode(mynode);
}
/* ---------------------------------------------------------------------- */
void ReadThread(Broke* const b, shared_ptr<Node> mynode) {
    mynode->ProtectThread.lock();
    printf("[%d] start readthread\n", mynode->GetIndex());
    mynode->ProtectThread.unlock();

    int connectFd = mynode->GetConnFd();
    bool MsgFlag = true;
    mynode->SetState(_online);
    while (MsgFlag && mynode->GetState() != _close) {
        shared_ptr<char> buffer(new char[MAX_BUFFER_SIZE]);
        // 获取头部---------------------------------------------------------
        int RecvHeadLen = headlength;
        Head head;
        int MsgLen = 0;
        while (MsgFlag && RecvHeadLen > 0) {
            MsgLen = read(connectFd, buffer.get(), RecvHeadLen);
            if (MsgLen > 0)
                RecvHeadLen -= MsgLen;
            else
                MsgFlag = false;
        }
        if (MsgFlag) {
            GetHead(head, buffer.get());
        }
        else {
            continue;
        }
        // 获取数据---------------------------------------------------------
        int RecvTopicNameLen = head.topic_name_len;
        int RecvDataLen = head.data_len;
        int RecvBodylen = RecvTopicNameLen + RecvDataLen;
        while (MsgFlag && RecvBodylen > 0) {
            MsgLen = read(connectFd, buffer.get()+headlength, RecvBodylen);
            if (MsgLen > 0)
                RecvBodylen -= MsgLen;
            else
                MsgFlag = false;
        }
        out((uint8_t *)buffer.get(), headlength+head.topic_name_len+head.data_len);
        // 处理数据---------------------------------------------------------
        assert(head.check_code == codeGenera(buffer.get()+8, head.topic_name_len+head.data_len));
        if (MsgFlag) {
            b->MsgHandler(mynode, buffer, head);
        }
        else {
            continue;
        }
    }
    if (mynode->GetState() != _close) mynode->SetState(_close);
    mynode->CloseWrite();
    printf("[%d] read stop\n", mynode->GetIndex());
}
/* ---------------------------------------------------------------------- */
void WriteThread(Broke* const b, shared_ptr<Node> mynode) {
    mynode->ProtectThread.lock();
    printf("[%d] start writethread\n", mynode->GetIndex());
    mynode->ProtectThread.unlock();
    int connectFd = mynode->GetConnFd();
    while (1) {
        // 等待被唤醒
        mynode->WaitForSendMsg();
        // 发送
        if (mynode->GetState() != _close) {
            Msg msg = mynode->GetTopMsg();
            write(connectFd, msg.buffer.get(), headlength + msg.head.topic_name_len + msg.head.data_len);
        }
        else {
            break;
        }
    }
    printf("[%d] write stop\n", mynode->GetIndex());
}
/* ---------------------------------------------------------------------- */
void MsgCopyToNode(Broke* const b, Msg msg, string topicname, uint32_t nodeindex) {
    // 得到订阅集合
    uint32_t nodeset = b->GetSubNodeSet(topicname, nodeindex);
    uint32_t a = 0x00000001;
    while (a != 0) {
        if ((a & nodeset) != 0) {
            shared_ptr<Node> nodeptr = b->GetNodePtr(a);
            if (nodeptr) {
                nodeptr->SendData(msg);
            }
        }
        a = a << 1;
    }
}
#endif