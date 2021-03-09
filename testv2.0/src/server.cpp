#include "../include/header.h"

void ConnectHandle(Master&, int, struct sockaddr_in);

class Node {
public:
    Node() = default;
    Node(string ip, int port) : Nip(ip), Nport(port) {}
    Node(const Node& node) : Nip(node.Nip), Nport(node.Nport), ConnectFd(node.ConnectFd), NodeMsgQueue(node.NodeMsgQueue) {}
private:
    uint32_t Index;
    string Nip;
    int Nport;
    int ConnectFd;
    State state = state_close;
    queue<Data> NodeMsgQueue;
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
    queue<Data> MsgQueue;
    unordered_map<string, Node> NodeList;
    vector<Topic> TopicList; 
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
    /*
    服务器连接到节点之后的处理函数
    1. 接收节点注册信息 --- 存储信息，返回节点编号
    */
    cout << "start handle new connect!\n";
    cout << connectFd << "--";
    cout << inet_ntoa(clientaddr.sin_addr) << ":";
    cout << clientaddr.sin_port << endl;
    char *buffer = new char[265];
    int RecvMsgLen = 0;
    while (1) {
        RecvMsgLen = read(connectFd, buffer, headlength);
        if (RecvMsgLen <= 0) break;
        else {
            if (RecvMsgLen >= headlength) {
                // 分析头部
                // out((uint8_t*)buffer, RecvMsgLen);
                Head head;
                GetHead(head, buffer);
                cout << head.data_len << endl;
                int RecvMsgLen2 = read(connectFd, buffer + RecvMsgLen, head.data_len);
                out((uint8_t*)buffer, RecvMsgLen + RecvMsgLen2);
            }
            else {
                // 接收剩下的头
                int remain = headlength - RecvMsgLen;
                RecvMsgLen = read(connectFd, buffer+RecvMsgLen, remain);
                if (RecvMsgLen >= remain) {
                    // 分析头部
                    out((uint8_t*)buffer, RecvMsgLen);
                }
            }
        }
    }
    close(connectFd);
    delete []buffer;
    cout << "end connect!\n";
}

int main() {
    string ip = "127.0.0.1";
    int port = 8000;
    Master m(ip, port);
    m.ServerInit();
    m.ServerListen();
    signal(SIGINT, SigThread);
    while (KeepRunning);
    return 0;
}