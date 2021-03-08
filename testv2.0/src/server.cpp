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
#define state_close 0;
#define state_connect 1;
#define state_disconnect 2;
typedef int State;
class Master;

void ConnectHandle(Master&, int, struct sockaddr_in);

class Message {

};

class Node {
public:
    Node() = default;
    Node(string ip, int port) : Nip(ip), Nport(port) {}
    Node(const Node& node) : Nip(node.Nip), Nport(node.Nport), ConnectFd(node.ConnectFd), NodeMsgQueue(node.NodeMsgQueue) {}

private:
    string Nip;
    int Nport;
    int ConnectFd;
    State state = state_close;
    queue<Message> NodeMsgQueue;
};

class Master {
public:
    Master() {
        cout << "Master start!" << endl;
    }
    Master(string ip, int port) : Mip(ip), Mport(port) {
        cout << "Master start!" << endl;
    }
    ~Master() {
        close(SocketFd);
        cout << "Bye~ Master!" << endl;
    }
    void ServerInit();
    void ServerListen();
    void ConnectHandl();
    string MasterIp() { return string(Mip+to_string(Mport)); }
private:
    string Mip;
    int Mport;
    int SocketFd;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    queue<Message> MsgQueue;
    vector<Node> NodeList;
    vector<int> ConnectSocketList;
};
void Master::ServerInit() {
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
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(Mip.c_str());
    serveraddr.sin_port = htons(Mport);
    try
    {
        int bindflag = bind(SocketFd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
        if (bindflag == -1)
            throw ("Master bind false!");
        cout << "Master bind success!\n";
    }
    catch(const char* e)
    {
        std::cerr << e << '\n';
        exit(-1);
    }
}
void Master::ServerListen() {
    listen(SocketFd, 30);
    socklen_t clientsocketlen = sizeof(clientaddr);
    while (1)
    {
        int connectFd = accept(SocketFd, (struct sockaddr*)&clientaddr, &clientsocketlen);
        if (connectFd == -1) {
            cout << errno << " [accept wrong]" << '\n';
            continue;
        }
        
        thread t(ConnectHandle, ref(*this), connectFd, clientaddr);
        t.detach();
    }
}

void ConnectHandle(Master& m, int connectFd, struct sockaddr_in clientaddr) {
    // cout << "start handle new connect!\n";
    // cout << connectFd << endl;
    // cout << inet_ntoa(clientaddr.sin_addr) << endl;
    // cout << clientaddr.sin_port << endl;
    // char *buffer = new char[100];
    // int RecvMsgLen = 0;
    // while (!((RecvMsgLen = read(connectFd, buffer, sizeof(buffer))) <= 0))
    // close(connectFd);
    // delete []buffer;
    // cout << "end connect!\n";
    /*
    服务器连接到节点之后的处理函数
    1. 转发数据
    2. 接收数据
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
    string ip = "127.0.0.1";
    int port = 8000;
    Master m(ip, port);
    m.ServerInit();
    m.ServerListen();
    signal(SIGINT, SigThread);
    while (KeepRunning);
}