#include "../include/header.h"

void ConnectHandle(Client&, int);
string zhuce(Client&, string);

class Node {
public:
    Node() = default;
    Node(string ip, int port) : Nip(ip), Nport(port) {}
    Node(const Node& node) : 
    Nip(node.Nip), Nport(node.Nport), ConnectFd(node.ConnectFd), 
    SendMsgQueue(node.SendMsgQueue), RecvMsgQueue(node.RecvMsgQueue),
    DingYueTopic(node.DingYueTopic), FaBuTopic(node.FaBuTopic) {}

    uint32_t GetIndex() { return Index; }
private:
    uint32_t Index;
    string Nip;
    int Nport;
    int ConnectFd;
    mutex mut;
    queue<Data> SendMsgQueue;
    queue<Data> RecvMsgQueue;
    vector<string> DingYueTopic;
    vector<string> FaBuTopic;
};

class Client
{
private:
    string Ip;
    int Port;
    int SocketFd;
    struct sockaddr_in clientaddr;
public:
    Node MyNode;
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
        // exit(-1);
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
    /* 1.节点注册 --- 接收节点编号 */
    cout << socketfd << " connect server\n";
    string name("gyh-123");
    zhuce(c, name);
    close(socketfd);
    cout << socketfd << " disconnect server\n";
}
uint8_t codeGenera(string msg) {
    uint8_t res = 0;
    return res;
}
Head HeadGenera(uint32_t index, Data data, msg_type msgtype) {
    Head h;
    h.type = msgtype;
    h.check_code = codeGenera(data.data);
    uint8_t machine_num = msgtype == reg ? 0 : 1;
    while (index != 1 && msgtype != reg) {
        machine_num *= 2;
        index >> 1;
    }
    h.machine_num = machine_num;
    h.data_len = data.data.size();
    h.rand_num = rand()%256;
    h.data_type = data.DataType;
    
    cout << sizeof(h) << endl;

    return h;
}
string zhuce(Client& c, string msg) {
    string regmsg;
    Data data;
    data.DataType  = 0x00000000;
    data.DataType = data.DataType | 0x00000001 | ((uint8_t(msg.size()))<<8);    
    data.data = msg;

    cout << data.DataType << endl;

    Head h = HeadGenera(c.MyNode.GetIndex(), data, reg);
    return regmsg;
}
Data MessageGenera() {

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
    return 0;
}