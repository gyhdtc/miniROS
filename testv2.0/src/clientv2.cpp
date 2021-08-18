#include "../include/clienthead.h"
// 连接server，管理线程
void ManageClient(MyNode* const, shared_ptr<Node>);
// 新节点的 读/写 线程
void ReadThread(MyNode* const, shared_ptr<Node>);
void WriteThread(MyNode* const, shared_ptr<Node>);
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
class Client
{
private:
    int socketFd;
    struct sockaddr_in dest_addr;
    int serverPort;
    string serverIp;
public:
    int ConnectBroke();
    Client(string ip, int port) : serverIp(ip), serverPort(port) {
        printf("Create a client , server = %s:%d\n", serverIp.c_str(), serverPort);
    }
};
int Client::ConnectBroke() {    
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        printf("Error: socket\n");
        return 0;
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(serverPort);
    dest_addr.sin_addr.s_addr = inet_addr(serverIp.c_str());
    if (connect(socketFd, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
        printf("Error : connect\n");
        return 0;
    }
    return socketFd;
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
class Node
{
private:
    int Fd;
    // 状态
    int State;
    mutex StateLock;
    condition_variable CloseCv;
    // 节点序号
    int32_t nodeIndex;
    mutex IndexLock;
    // 数据传输后，不变
    string nodeName;
    // 节点 订阅/发布 的话题
    set<string> subTopicList;
    set<string> pubTopicList;
    // 数据消息
    deque<Msg> Message;
    mutex MsgLock;
    sem_t Msg_Sem;
public:
    int GetSocketFd();
    string GetName();
    // 设置节点
    void SetIndex(int32_t);
    int32_t GetIndex();
    // 节点状态设置
    int GetState();
    void SetState(int);
    // 等待节点关闭
    void WaitForClose();
    void CloseWrite();
    // 发送
    void SendReg(Msg);// 发送注册信息
    void SendDate(Msg);// 发送数据
    void SendTopic(Msg);// 发送有关话题的数据
    void SendMsg();// 等待发送数据
    Msg GetTopMsg();// 获取队伍头部数据
    // 处理话题信息
    void AddSub(string name) { subTopicList.insert(name); }
    void AddPub(string name) { pubTopicList.insert(name); }
    void deltopic(string);
    Node(string _name, int _fd) : nodeName(_name), Fd(_fd) {
        State = _newconnect;
        printf("Create a node = %s\n", nodeName.c_str());
    }
};
string Node::GetName() { return nodeName; }
int Node::GetSocketFd() { return Fd; }
void Node::SetIndex(int32_t t) { 
    unique_lock<mutex> lk(IndexLock);
    nodeIndex = t;
}
int32_t Node::GetIndex() {
    unique_lock<mutex> lk(IndexLock);
    return nodeIndex;
}
void Node::SetState(int transferstate) {
    unique_lock<mutex> lk(StateLock);
    if (state_transfer[State][transferstate] == 1) {
        State = transferstate;
        printf("Set Node-%d State to %s\n", nodeIndex, DP[transferstate].c_str());
        if (State == _close) CloseCv.notify_one();
        return;
    }
    return;
}
int Node::GetState() {
    unique_lock<mutex> lk(StateLock);
    return State;
}
void Node::WaitForClose() {
    unique_lock<mutex> lk(StateLock);
    CloseCv.wait(lk, [this](){ return State == _close; });
}
void Node::deltopic(string name) {
    if (pubTopicList.find(name) != pubTopicList.end()) {
        pubTopicList.erase(name);
    }
    else if (subTopicList.find(name) != subTopicList.end()) {
        subTopicList.erase(name);
    }
    else {
        printf("No this topic\n");
    }
}
void Node::CloseWrite() {
    sem_post(&Msg_Sem);
}
void Node::SendMsg() {
    sem_wait(&Msg_Sem);
}
Msg Node::GetTopMsg() {
    unique_lock<mutex> lk(MsgLock);
    Msg msg = Message.front();
    Message.pop_front();
    return msg;
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
class MyNode
{
private:
    shared_ptr<Node> node;
    Client* client;
public:
    mutex ProtectThread;
    void StartClient(string);
    void Reg();
    void AddSub(string);
    void AddPub(string);
    void DelTopic(string);
    void MsgHandler(shared_ptr<Node>, shared_ptr<char>, Head);
    MyNode(string _ip, int _port) {
        client = new Client(_ip, _port);
        printf("Start Client\n");
    }
};
void MyNode::StartClient(string name) {
    int fd;
    if (fd = client->ConnectBroke()) {
        printf("Success start client\n");
        node = make_shared<Node>(Node(name, fd));
        thread t(ManageClient, this, node);
        t.detach();
    }
    else {
        printf("fail start client\n");
    }
}
void MyNode::Reg() {

}
void MyNode::MsgHandler(shared_ptr<Node> mynode, shared_ptr<char> buffer, Head head) {
    Msg msg;
    switch (head.type)
    {
        case heartbeat:
        {
            printf("get heartbeat\n");
            
            break;
        }
        case getheartbeat:
        {
            printf("get getheartbeat\n");
            
            break;
        }
        // 对于 topic 的操作其实应该开线程，否则客户端一多就会堵
        case subtopic:
        {
            printf("get subtopic\n");
            
            break;
        }
        case pubtopic:
        {
            printf("get pubtopic\n");
            
            break;
        }
        case deltopic:
        {
            printf("get deltopic\n");
            
            break;
        }
        // 对于 topic 的操作其实应该开线程，否则客户端一多就会堵
        case regnode:
        {
            printf("get regnode\n");
            
            break;
        }
        case getregnode:
        {
            printf("get getregnode\n");
            // broke 按道理是不会受到这个的
            break;
        }
        case reconnect:
        {
            printf("get reconnect\n");
            // 未完待续
            break;
        }
        case data:
        {
            printf("get data\n");
            
            break;
        }
        default:
        {
            printf("error message head\n");
            break;
        }
    }
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
void ManageClient(MyNode* const mynode, shared_ptr<Node> node) {
    // 创建 读/写 线程；
    node->SetState(_connecting);
    mynode->ProtectThread.lock();
    thread t1(ReadThread, mynode);
    t1.detach();
    mynode->ProtectThread.unlock();
    thread t2(WriteThread, mynode);
    t2.detach();
    // 注册
    mynode->Reg();
    node->WaitForClose();
}
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
        while (MsgFlag && RecvHeadLen > 0) {
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
        while (MsgFlag && RecvBodylen > 0) {
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
void WriteThread(MyNode* const mynode, shared_ptr<Node> node) {
    mynode->ProtectThread.lock();
    printf("node %d start writethread\n", node->GetName());
    mynode->ProtectThread.unlock();
    int connectFd = node->GetSocketFd();
    while (1) {
        // 等待被唤醒
        node->SendMsg();
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
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
int main() {
    // 本节点的名字
    string myname;
    cout << "input node name:\n";
    cin >> myname;
    // broke 代理的 ip 和 port
    string serverip = "127.0.0.1";
    int serverport = 8787;
    // 创建一个 mynode 类
    MyNode* mynode = new MyNode(serverip, serverport);
    mynode->StartClient(myname);
    while (KeepRunning);
    delete mynode;
    return 0;
}