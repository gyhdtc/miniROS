#ifndef SERVER
#define SERVER
#include "./server_head.h"
/* ---------------------------------------------------------------------- */
class Server
{
private:
    int listenfd;
    int myport;
    string myIp;
public:
    void Bind();
    void Listen();
    int Accept(struct sockaddr_in&);
    Server(string ip, int port)
    {
        myIp = ip;
        myport = port;   
        printf("Create a server = %s:%d\n", myIp.c_str(), myport);
    }
    ~Server()
    {
        close(listenfd);
    }
};
/* ---------------------------------------------------------------------- */
class Topic
{
private:
    map<int32_t, set<string>> sub2name; // 发布节点序号 --- 发布话题名称s
    mutex SubLock;
    map<int32_t, set<string>> pub2name; // 发布节点序号 --- 订阅话题名称s
    mutex PubLock;
    map<string, pair<int32_t, int32_t>> name2subANDpub; // 名字 --- （订阅节点，发布节点）
    mutex NameLock;
public:
    void subTopic(string, int32_t);
    void pubTopic(string, int32_t);
    void delTopic(string, int32_t);
    void printName(string);
    void printIndex(uint32_t);
    void printAll();
    uint32_t GetSubNodeSet(string, uint32_t);
};
/* ---------------------------------------------------------------------- */
class Node
{
private:
    // 状态
    int State;
    mutex StateLock;
    // 关闭状态的条件变量
    condition_variable CloseCv;
    // 节点序号
    int32_t nodeIndex;
    bool IndexFlag;
    bool TimeOutflag;
    mutex IndexLock;
    // 序号重获的条件变量
    condition_variable IndexResetCv;
    // 连接后，不变
    string nodeIp;
    int nodePort;
    int connectfd;
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
    // 在创建多线程的时候，用来保护node数据不出错
    // 有时候也用来保护 shared ptr 的引用记数正确
    mutex ProtectThread;
    int GetConnFd() const { return connectfd; }
    // 节点序列号设置
    int32_t GetIndex();
    bool ResetIndex(int);
    int SetIndex(int32_t);
    // 节点状态设置
    int GetState();
    void SetState(int);
    // 等待节点关闭
    void WaitForClose();
    // 发送函数
    void WaitForSendMsg();
    void CloseWrite();
    void SendCheckHeart(Msg);
    void SendCheckReg(Msg);
    void SendData(Msg);
    void SetName(string name) { nodeName = name; }
    Msg GetTopMsg();
    // 处理话题信息
    void AddSub(string name) { subTopicList.insert(name); }
    void AddPub(string name) { pubTopicList.insert(name); }
    void deltopic(string);
    Node(string ip, int port, int fd)
    {
        nodeIp = ip;
        nodePort = port;
        connectfd = fd;
        nodeIndex = 0x00000000;
        State = _newconnect;
        IndexFlag = false;
        TimeOutflag = false;
        // 初始化信号量
        int ret = sem_init(&Msg_Sem, 0, 0);
        printf("Create a node\n");
    }
    ~Node()
    {
        printf("[%d] Close a Node = %s:%d %s\n", nodeIndex, nodeIp.c_str(), nodePort, nodeName.c_str());
        // close(connectfd);
        shutdown(connectfd, SHUT_RDWR);
        sem_destroy(&Msg_Sem);
    }
};
/* ---------------------------------------------------------------------- */
class Broke
{
private:
    int32_t totalNode;
    mutex IndexLock;
    Server* server;
    Topic* topic;
    map<int32_t, shared_ptr<Node>> index2nodeptr;
    mutex Index2NodeptrLock;
    // 数据有关的成员
    deque<Msg> Message;
    mutex MsgLock;
    condition_variable MsgCv;
public:
    // 创建连接
    void StartServer(string, int);
    int WaitAccpet(struct sockaddr_in&);
    // 管理节点
    int32_t AllocIndex();
    bool AddNode(shared_ptr<Node>);
    bool DelNode(shared_ptr<Node>);
    // 处理数据的函数
    void MsgHandler(shared_ptr<Node>, shared_ptr<char>, Head);
    uint32_t GetSubNodeSet(string, uint32_t);
    shared_ptr<Node> GetNodePtr(uint32_t);
    Broke()
    {
        totalNode = 0;
        topic = new Topic;
        server = NULL;
        printf("Start broke\n");
    }
    ~Broke()
    {
        delete topic;
        delete server;
        printf("Stop broke\n");
    }
};
#endif