#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <errno.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory.h>
#include <signal.h>
#include <bitset>
#include <stdarg.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
using namespace std;
// 定义常量
#define IPADDRESS   "127.0.0.1"
#define PORT        8787
#define MAXSIZE     1024
#define LISTENQ     100
// 定义 node 状态
#define _newconnect 1
#define _connecting 2
#define _connected 3
#define _connect_wait 4
#define _close 5
#define _close_1 6
int state_transfer[6][6] = 
{
    0,0,0,0,0,0,
    0,1,1,0,1,0,
    0,0,1,1,0,0,
    0,0,0,1,0,1,
    0,0,1,0,1,1,
    0,0,0,0,0,1
};
/* 声明一些类 */
class Server;
class Node;
class Topic;
class Broke;
/* 声明一些线程函数 */
// 等待连接线程
void AccpetThread(Broke*);
// 处理/管理 新连接
void ConnectThread(Broke*, int, sockaddr_in);
// 新节点的 读/写 线程
void ReadThread(Broke*, Node*);
void WriteThread(Broke*, Node*);

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
void Server::Bind() {
    const char * ip = myIp.c_str();
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket error\n");
        exit(1);
    }
    printf("Create a listenfd , fd : %d\n", listenfd);
    // 清空结构体
    memset(&serveraddr, 0, sizeof(serveraddr));
    // 给结构体赋值，转换格式
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(myport);
    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind error\n");
        exit(1);
    }
    printf("bind success\n");
}
void Server::Listen() {
    listen(listenfd, LISTENQ);
}
int Server::Accept(struct sockaddr_in& cliaddr) {
    int clifd;
    socklen_t cliaddrlen = sizeof(cliaddr);
    clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
    return clifd;
}

class Node
{
private:
    int State;
    mutex StateLock;
    condition_variable CloseCv;
    string nodeIp;
    int nodePort;
    string nodeName;
    int32_t nodeIndex;
    int connectfd;
    vector<string> subTopicList; // 节点订阅的话题
    vector<string> pubTopicList; // 节点发布的话题
public:
    int GetState();
    bool SetIndex(int32_t);
    void SetState(int);
    void WaitForClose();
    Node(string ip, int port, int fd)
    {
        nodeIp = ip;
        nodePort = port;
        connectfd = fd;
        nodeIndex = 0x00000000;
        State = _newconnect;
        printf("Create a node = %s:%d\n", nodeIp.c_str(), nodePort);
    }
    ~Node()
    {
        printf("Close a Node = %s:%d\n", nodeIp.c_str(), nodePort);
        close(connectfd);
    }
};
bool Node::SetIndex(int32_t index) {
    if (nodeIndex == 0x00000000) {
        nodeIndex = index;
        return true;
    }
    return false;
}
void Node::SetState(int transferstate) {
    unique_lock<mutex> lk(StateLock);
    if (state_transfer[State][transferstate] == 1) {
        State = transferstate;
        if (State == _close) {
            CloseCv.notify_one();
        }
        return;
    }
    return;
}
void Node::WaitForClose() {
    unique_lock<mutex> lk(StateLock);
    while (State != _close) {
        CloseCv.wait(lk);
    }
}
int Node::GetState() {
    unique_lock<mutex> lk(StateLock);
    return State;
}

class Topic
{
private:
    map<string, pair<int32_t, int32_t>> name2subANDpub; // 名字 --- （订阅节点，发布节点）
public:
    void subTopic(string, int32_t);
    void pubTopic(string, int32_t);
    void delTopic(string, int32_t);
};

class Broke
{
private:
    int32_t totalNode;
    mutex IndexLock;
    Server* server;
    Topic* topic;
    map<int32_t, Node*> index2nodeptr;
public:
    void StartServer(string, int);
    int WaitAccpet(struct sockaddr_in&);
    int32_t AllocIndex();
    Broke()
    {
        totalNode = 0;
        topic = new Topic;
        server = NULL;
        printf("Start broke\n");
    }
    ~Broke()
    {
        for (auto i : index2nodeptr) delete i.second;
        delete topic;
        delete server;
        printf("Stop broke\n");
    }
};
void Broke::StartServer(string ip, int port) {
    if (server == NULL) {
        server = new Server(ip, port);
    }
    server->Bind();
    server->Listen();
    AccpetThread(this);
}
int Broke::WaitAccpet(struct sockaddr_in& cliaddr) {
    int connectfd = server->Accept(cliaddr);
    return connectfd;
}
int32_t Broke::AllocIndex() {
    lock_guard<mutex> guard(IndexLock);
    int32_t t = 0x00000001;
    while ((t != 0) && ((totalNode & t) != 0x00000000)) t = t << 1;
    totalNode = totalNode | t;
    return t;
}

void AccpetThread(Broke* b) {
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t len;
    len = sizeof(cliaddr);
    while (1) {
        printf("Wait a new client...\n");
        clifd = b->WaitAccpet(cliaddr);
        if (clifd == -1)
            perror("accpet error:");
        else
        {
            printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
            // 新的客户端连接
            thread t(ConnectThread, b, clifd, cliaddr);
            t.detach();
        }
        break;
    }
}

void ConnectThread(Broke* b, int connectfd, struct sockaddr_in cliaddr) {
    Node* mynode = new Node(inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port, connectfd);
    int32_t index_temp = b->AllocIndex();
    if (index_temp <= 0) {
        // 序号分配出错
        mynode->SetState(_connect_wait);
    }
    else {
        // 序号分配正确
        if (mynode->SetIndex(index_temp)) {
            // 序号设置正确
            mynode->SetState(_connecting);
        }
        else {
            // 序号设置错误
            mynode->SetState(_connect_wait);
        }
    }
    switch (mynode->GetState())
    {
    case _connecting:
    {
        // 开启 读写 线程
        thread t1(ReadThread, b, mynode);
        thread t2(WriteThread, b, mynode);
        mynode->SetState(_connected);
        t1.detach();
        t2.detach();
        break;
    }
    case _connect_wait:
        // 设置超时时间
        // 期间一直尝试 获取序号/分配序号
        // int func(Broke*, Node*)
        // 这个函数返回 0/1，0 表示超时了
        break;
    default:
        printf("something error happened in [ConnectThread, switch]\n");
        delete mynode;
        break;
    }
    mynode->WaitForClose();
    delete mynode;
}
void ReadThread(Broke* b, Node* node) {

}

void WriteThread(Broke* b, Node* node) {
    
}
int main() {
    int sockfd;
    struct sockaddr_in  servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    sleep(10);
    //处理连接
    close(sockfd);
    return 0;
}