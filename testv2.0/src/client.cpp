#include "../include/header.h"

void ConnectHandle(Client&, int);
void zhuce(Client&, int);

class Node {
public:
    Node() = default;
    Node(string ip, int port) : Nip(ip), Nport(port) {}
    // Node(const Node& node) : Nip(node.Nip), Nport(node.Nport), ConnectFd(node.ConnectFd), SendMsgQueue(node.SendMsgQueue), RecvMsgQueue(node.RecvMsgQueue), DingYueTopic(node.DingYueTopic), FaBuTopic(node.FaBuTopic) {}
    uint32_t GetIndex() { return Index; }
private:
    uint32_t Index = 0x00000000;
    string NodeName;
    string Nip;
    int Nport;
    int ConnectFd;
    mutex mut;
    // 发送的是编码好的，带头的
    queue<msg_packet> SendMsgQueue;
    // 发送数据的缓存，int --- msg_packet 的映射，收到对应的ack则删除
    unordered_map<int, msg_packet> ReSendMsgCache;
    // 接收的应该是解码好的 Data
    queue<Data> RecvMsgQueue;
    unordered_map<string, uint8_t> DingYueTopic;
    unordered_map<string, uint8_t> FaBuTopic;
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
    Client() { cout << "Client start!" << endl; }
    Client(string ip, int port) : Ip(ip), Port(port) { cout << "Client start!" << endl; }
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

void ConnectHandle(Client& c, int SocketFd) {
    /* 1.节点注册 ---> 接收节点编号 */
    cout << SocketFd << " connect server\n";
    zhuce(c, SocketFd);
    close(SocketFd);
    cout << SocketFd << " disconnect server\n";
}
// 数据流生成器，需要多个重载，此重载只针对 string 类型数据流
void DataGenera(Data& d, string s) {
    if (s.size() >= 255) s = s.substr(0, 255);
    for (int i = 0; i < s.size(); ++i) {
        d.charlist.push_back(s[i]);
        d.databytestream.push_back(uint8_t(s[i]));
    }
}
// 节点注册：需要 本机当前序列号，本机名字, 数据类型（普通、图像、坐标）
// 节点注册：返回一个 vector<uint8_t> 类型的 msg_packet，用来传输
void zhuce(Client& c, int SocketFd) {
    cout << "[注册]\n";

    msg_packet mp;
    Data data;
    string name("gyh-1");
    uint32_t index = c.MyNode.GetIndex();

    DataGenera(data, name);
    DataTypeGenera(data);
    vector<uint8_t> headbytestream = HeadGenera(index, data, reg);
    if (headbytestream.size() != 5) {
        cout << "head genera false\n";
        return;
    }
    Combine(mp, headbytestream);
    Combine(mp, data.DataType);
    Combine(mp, vector<uint8_t>(1,0x00));
    Combine(mp, data.databytestream);

    char* t = new char[mp.size()];
    write(SocketFd, (const uint8_t*)&mp[0], mp.size());
    out((uint8_t*)&mp[0], mp.size());
    return;
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