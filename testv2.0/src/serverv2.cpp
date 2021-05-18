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
#include <memory>
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
using namespace std;

class Server
{
private:
    int listenfd;
    string myIp;
    int myPort;
public:
    // Server();
    Server(string, int);
    ~Server();
};

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

class Node
{
private:
    string nodeIp;
    int nodePort;
    string nodeName;
    int32_t nodeIndex;
    int connectfd;
    vector<string> subTopicList; // 节点订阅的话题
    vector<string> pubTopicList; // 节点发布的话题
public:
    // Node();
    void serverbind();
    void serverlisten();    
    Node(string, int, int);
    ~Node();
};

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
    void startserver(string, int);
    void addNode(Node*);
    void delNode(int32_t);
    
    Broke();
    ~Broke();
};
void Broke::startserver(string ip, int port)
{
    server = new Server(ip, port);
}
Broke::Broke()
{
    totalNode = 0;
    topic = new Topic;
    cout << "Start broke\n";
}

Broke::~Broke()
{
    for (auto i : index2nodeptr) delete i.second;
    delete topic;
    delete server;
    cout << "Stop broke\n";
}
