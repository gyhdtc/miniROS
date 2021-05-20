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
#define IPADDRESS   "127.0.0.1"
#define PORT        8787
#define MAXSIZE     1024
#define LISTENQ     5
#define FDSIZE      1000
#define EPOLLEVENTS 100

void AccpetThread(Broke*);
void ReadThread(Broke*, int, struct sockaddr_in);

class Server
{
private:
    int listenfd;
    int myPort;
    string myIp;
public:
    void Bind();
    int Accept(struct sockaddr_in&);
    Server::Server(string ip, int port)
    {
        myIp = ip;
        myPort = port;   
        printf("Create a server = %s:%d\n", myIp, myPort);
    }
    Server::~Server()
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
    // 清空结构体
    memset(&serveraddr, 0, sizeof(serveraddr));
    // 给结构体赋值，转换格式
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(myPort);
    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind error\n");
        exit(1);
    }
}
int Server::Accept(struct sockaddr_in& cliaddr) {
    int clifd;
    socklen_t  cliaddrlen;
    clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
    return clifd;
}

class Node
{
private:
    int State;
    string nodeIp;
    int nodePort;
    string nodeName;
    int32_t nodeIndex;
    int connectfd;
    vector<string> subTopicList; // 节点订阅的话题
    vector<string> pubTopicList; // 节点发布的话题
public:
    Node::Node(string ip, int port, int fd)
    {
        nodeIp = ip;
        nodePort = port;
        connectfd = fd;
        printf("Create a node = %s:%d\n", ip, port);
    }
    Node::~Node()
    {
        close(connectfd);
    }
};

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
    Server* server;
    Topic* topic;
    map<int32_t, Node*> index2nodeptr;
public:
    void StartServer(string, int);
    int WaitAccpet(struct sockaddr_in&);

    Broke::Broke()
    {
        totalNode = 0;
        topic = new Topic;
        server = NULL;
        cout << "Start broke\n";
    }
    Broke::~Broke()
    {
        for (auto i : index2nodeptr) delete i.second;
        delete topic;
        delete server;
        cout << "Stop broke\n";
    }
};
void Broke::StartServer(string ip, int port) {
    printf("Create a listenfd ...\n");
    if (server == NULL) {
        server = new Server(ip, port);
    }
    server->Bind();
    AccpetThread(this);
}
int Broke::WaitAccpet(struct sockaddr_in& cliaddr) {
    int connectfd = server->Accept(cliaddr);
    return connectfd;
}


void AccpetThread(Broke* b) {
    int clifd;
    struct sockaddr_in cliaddr;
    while (1) {
        printf("Wait a new client...\n");
        clifd = b->WaitAccpet(cliaddr);
        if (clifd == -1)
            perror("accpet error:");
        else
        {
            printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
            // 新的客户端连接
            // 给他创建两个线程，分别 读/写
            thread t(ReadThread, b, clifd, cliaddr);
            t.detach();
        }
    }
}
