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

    /* 开启数据转发线程 */
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
    cout << "start handle new connect!\n";
    cout << connectFd << "--" << inet_ntoa(clientaddr.sin_addr) << ":" << clientaddr.sin_port << endl;
    char *buffer = new char[MAX_BUFFER_SIZE];
    while (1) {
        int RecvMsgLen = 0;
        shared_ptr<char> real_msg(new char[MAX_BUFFER_SIZE]);
        Head head;
        int RecvMsgLen1 = read(connectFd, buffer, headlength);
        if (RecvMsgLen1 <= 0)
            break;
        else {
            if (RecvMsgLen1 >= headlength) {
                // 分析头部
                // out((uint8_t*)buffer, RecvMsgLen);
                GetHead(head, buffer);
                // cout << head.data_len << endl;
                int RecvMsgLen2 = read(connectFd, buffer + RecvMsgLen, head.data_len - (RecvMsgLen - headlength));
                out((uint8_t*)buffer, RecvMsgLen1 + RecvMsgLen2);
                // 目前为止，buffer中存储了10字节的头部，和所有的消息
                // 总字节长度 ： RecvMsgLen1 + RecvMsgLen2
                RecvMsgLen = RecvMsgLen1 + RecvMsgLen2;
            }
            else {
                // 接收剩下的头
                int remain = headlength - RecvMsgLen;
                int RecvMsgLen2 = read(connectFd, buffer + RecvMsgLen1, remain);
                if (RecvMsgLen2 >= remain) {
                    // 分析头部
                    // out((uint8_t*)buffer, RecvMsgLen);
                    GetHead(head, buffer);
                    int RecvMsgLen3 = read(connectFd, buffer + RecvMsgLen1 + RecvMsgLen2, head.data_len - (RecvMsgLen1 + RecvMsgLen2 - headlength));
                    out((uint8_t*)buffer, RecvMsgLen1 + RecvMsgLen2 + RecvMsgLen3);
                    // 目前为止，buffer中存储了10字节的头部，和所有的消息
                    // 总字节长度 ： RecvMsgLen1 + RecvMsgLen2 + RecvMsgLen3
                    RecvMsgLen = RecvMsgLen1 + RecvMsgLen2 + RecvMsgLen3;
                }
            }
        }
        /* 接下来应该对消息进行处理 */
        /* 现将数据复制到 real_msg 指针（报文和头部分开），再对buffer清零，再开启一个线程进行处理 */
        memcpy(real_msg.get(), buffer+headlength, RecvMsgLen-headlength);
        thread t(MsgHandle, head, real_msg);
        t.detach();
    }
    close(connectFd);
    delete []buffer;
    cout << "end connect!\n";
}
void MsgHandle(Head head, shared_ptr<char> a) {
    /* 这里应该根据报文头部第一字节的数据 */
    /* 对报文进行一个详细的划分 */
    /* 1.首先应该返回一个确认收到的响应 */
    /* 2.如果是节点订阅话题、发布话题和注册，应该先将他们加入map，生成识别码，并返回 */
    /* 3.如果是心跳包，则返回收到心跳包响应 */
    /* 4.如果是数据，则保存下来之后，加入到数据队列中，由那个数据队列线程进行分发 */
    
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