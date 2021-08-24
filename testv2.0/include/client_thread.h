#ifndef CLIENTTHREAD
#define CLIENTTHREAD
#include "./client_head.h"
#include "./client_class.h"
// 连接server，管理线程
void ManageClient(MyNode* const, shared_ptr<Node>);
// 新节点的 读/写 线程
void ReadThread(MyNode* const, shared_ptr<Node>);
void WriteThread(MyNode* const, shared_ptr<Node>);
/* ---------------------------------------------------------------------- */
void ManageClient(MyNode* const mynode, shared_ptr<Node> node) {
    // 创建 读/写 线程；
    node->SetState(_connecting);
    mynode->ProtectThread.lock();
    thread t1(ReadThread, mynode, node);
    t1.detach();
    mynode->ProtectThread.unlock();
    thread t2(WriteThread, mynode, node);
    t2.detach();
    node->WaitForClose();
}
/* ---------------------------------------------------------------------- */
void ReadThread(MyNode* const mynode, shared_ptr<Node> node) {
    mynode->ProtectThread.lock();
    printf("node %s start readthread\n", node->GetName().c_str());
    mynode->ProtectThread.unlock();
    // 读取注册
    int fd = node->GetSocketFd();
    bool MsgFlag = true;
    while (MsgFlag && node->GetState() != _close) {
        shared_ptr<char> buffer(new char[MAX_BUFFER_SIZE]);
        // 获取头部---------------------------------------------------------
        int RecvHeadLen = headlength;
        Head head;
        int MsgLen = 0;
        while (MsgFlag && RecvHeadLen > 0 && node->GetState() != _close) {
            MsgLen = read(fd, buffer.get(), RecvHeadLen);
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
        while (MsgFlag && RecvBodylen > 0 && node->GetState() != _close) {
            MsgLen = read(fd, buffer.get()+headlength, RecvBodylen);
            if (MsgLen > 0)
                RecvBodylen -= MsgLen;
            else
                MsgFlag = false;
        }
        out((uint8_t *)buffer.get(), headlength+head.topic_name_len+head.data_len);
        // 处理数据---------------------------------------------------------
        assert(head.check_code == codeGenera(buffer.get()+8, head.topic_name_len+head.data_len));
        if (MsgFlag) {
            mynode->MsgHandler(node, buffer, head);
        }
        else {
            continue;
        }
    }
    if (node->GetState() != _close) node->SetState(_close);
    node->CloseWrite();
    printf("read stop\n");
}
/* ---------------------------------------------------------------------- */
void WriteThread(MyNode* const mynode, shared_ptr<Node> node) {
    mynode->ProtectThread.lock();
    printf("node %s start writethread\n", node->GetName().c_str());
    mynode->ProtectThread.unlock();
    node->SetState(_connected);
    int connectFd = node->GetSocketFd();
    while (1) {
        // 等待被唤醒
        node->WaitForSendMsg();
        // 发送
        if (node->GetState() != _close) {
            Msg msg = node->GetTopMsg();
            write(connectFd, msg.buffer.get(), headlength + msg.head.topic_name_len + msg.head.data_len);
        }
        else {
            break;
        }
    }
    printf("write stop\n");
}
#endif