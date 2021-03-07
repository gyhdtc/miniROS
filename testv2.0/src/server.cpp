#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>
#include <errno.h>
#include <queue>
#include <string>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
using namespace std;

class Master;

void ConnectHandle(Master&);

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
    queue<Message> NodeMsgQueue;
};

class Master {
typedef int socket_type;
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
    vector<socket_type*> ConnectSocketList;
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
    }
}
void Master::ServerListen() {
    listen(SocketFd, 30);
    socklen_t clientsocketlen = sizeof(clientaddr);
    while (1)
    {
        // thread t(ConnectHandle, ref(*this));
        // t.detach();
        int connectFd = accept(SocketFd, (struct sockaddr*)&clientaddr, &clientsocketlen);
        if (connectFd == -1) {
            cout << errno << " [accept wrong]" << '\n';
            continue;
        }
        cout << "new connect!\n";
        thread t(ConnectHandle, ref(*this));
        t.detach();
        // thread t(&Master::ConnectHandl, this);
    }
}
void ConnectHandle(Master& m) {
    cout << m.MasterIp() << '\n';
}
int main() {
    string ip = "127.0.0.1";
    int port = 8000;
    Master m(ip, port);
    m.ServerInit();
    m.ServerListen();
}