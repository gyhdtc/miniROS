#ifndef CLIENT
#define CLIENT
#include "./client_head.h"
/* ---------------------------------------------------------------------- */
class UserDataBase {
    public:
        // 类数据转string
        virtual string Class2String() const = 0;
        // string转类数据
        virtual void String2Class(string) = 0;
};
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
    ~Client() {
        printf("close a client\n");
        shutdown(socketFd, SHUT_WR);
        sleep(1);
    }
};
/* ---------------------------------------------------------------------- */
class Node
{
private:
    int Fd;
    int State;// 状态
    mutex StateLock;
    condition_variable CloseCv;
    int32_t nodeIndex;// 节点序号
    mutex IndexLock;
    string nodeName;// 数据传输后，不变
    set<string> subTopicList;// 节点 订阅 的话题
    set<string> pubTopicList;// 节点 发布 的话题
    deque<Msg> Message;// 发送数据消息队列
    deque<Msg> RecvMessage;// 接收数据消息队列
    mutex RecvLock;
    condition_variable RecvCv;
    mutex MsgLock;
    sem_t Msg_Sem;
public:
    int GetSocketFd() { return Fd; }
    string GetName() { return nodeName; }
    // 设置节点序号
    void SetIndex(int32_t t) { unique_lock<mutex> lk(IndexLock); nodeIndex = t; }
    int32_t GetIndex() { unique_lock<mutex> lk(IndexLock); return nodeIndex; }
    // 状态设置
    int GetState() { unique_lock<mutex> lk(StateLock); return State; }
    void SetState(int);
    void WaitForClose();// 等待节点关闭
    void CloseWrite();
    void SendReg(Msg&);// 发送
    void SendData(Msg&);
    void SendTopic(Msg&);
    void WaitForSendMsg();
    Msg GetTopMsg();
    void GetData(Msg);
    Msg WaitForData();
    bool AddSub(string);// 处理话题信息
    bool AddPub(string);
    bool deltopic(string);
    Node(string _name, int _fd) : nodeName(_name), Fd(_fd) {
        State = _newconnect;
        printf("Create a node = %s\n", nodeName.c_str());
    }
    ~Node() {
        cout << "pub :";
        for (auto i : pubTopicList) cout << i << " ";
        cout << endl;
        cout << "sub :";
        for (auto i : subTopicList) cout << i << " ";
        printf("\nClose a Node = %s\n", nodeName.c_str());
        sem_destroy(&Msg_Sem);
    }
};
/* ---------------------------------------------------------------------- */
class MyNode
{
private:
    shared_ptr<Node> node;
    shared_ptr<Client> client;
public:
    mutex ProtectThread;
    void StartClient(string);
    void Reg();
    void AddSub(string);
    void AddPub(string);
    void DelTopic(string);
    void SendData(string, string);
    Msg WaitForData();
    void MsgHandler(shared_ptr<Node>, shared_ptr<char>, Head);
    MyNode(string _ip, int _port) {
        client = make_shared<Client>(_ip, _port);
        printf("Start Client\n");
    }
    ~MyNode() {
        node->SetState(_close);
    }
};
#endif