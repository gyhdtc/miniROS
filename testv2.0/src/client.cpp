#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <errno.h>
#include <queue>
#include <signal.h>
#include <string>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
using namespace std;
int KeepRunning = 1;
class Client;

void ConnectHandle(Client&, int);

class Message {

};

class Node {
public:
    Node() = default;
    Node(string ip, int port) : Nip(ip), Nport(port) {}
    Node(const Node& node) : 
    Nip(node.Nip), Nport(node.Nport), ConnectFd(node.ConnectFd), 
    SendMsgQueue(node.SendMsgQueue), RecvMsgQueue(node.RecvMsgQueue),
    DingYue(node.DingYue), FaBu(node.FaBu) {}

private:
    string Nip;
    int Nport;
    int ConnectFd;
    queue<Message> SendMsgQueue;
    queue<Message> RecvMsgQueue;
    unordered_map<string, int> DingYue;
    unordered_map<string, int> FaBu;
};

class Client
{
private:
    string Ip;
    int Port;
    int SocketFd;
    struct sockaddr_in clientaddr;
    Node MyNode;
public:
    Client() {
        cout << "Client start!" << endl;
    }
    Client(string ip, int port) : Ip(ip), Port(port) {
        cout << "Client start!" << endl;
    }
    ~Client() {
        close(SocketFd);
        cout << "Bye~ Client!" << endl;
    }

    void ClientInit();
    void Connect();
};

void Client::ClientInit() {
    try
    {
        SocketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (SocketFd == -1)
        throw ("socket create false");
    }
    catch(const char* e)
    {
        std::cerr << e << '\n';
    }
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = inet_addr(Ip.c_str());
    clientaddr.sin_port = htons(Port);
}

void Client::Connect() {
    try
    {
        int connectflag = connect(SocketFd, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
        if (connectflag == -1)
            throw ("client connect false!");
        cout << "client connect success!\n";
    }
    catch(const char* e)
    {
        std::cerr << e << '\n';
        exit(-1);
    }

    thread t(ConnectHandle, ref(*this), SocketFd);
    t.detach();
}

void ConnectHandle(Client& c, int socketfd) {
    // cout << socketfd << " connect server\n";
    // close(socketfd);
    // cout << socketfd << " disconnect server\n";
    /*
    客户端连接上服务器之后的处理函数
    1. 心跳包，断线重连;
    2. 发送数据，client->mynode中的sendqueue非空就发送;
    3. 接收数据，存放到client->mynode中的recvqueue；
    4. 发送失败回调函数；
    5. 数据默认无序；
    6. 节点注册
    */
}

void SigThread(int sig) {
    if (sig == SIGINT || sig == SIGSTOP)
    {
        KeepRunning = 0;
        cout << endl;
    }
}
int main() {
    string mip = "127.0.0.1";
    int mport = 8000;
    Client c(mip, mport);
    c.ClientInit();
    c.Connect();
    signal(SIGINT, SigThread);
    while (KeepRunning);
}