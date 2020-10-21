#ifndef A
#define A
#include "header.h"
#endif
#include "server.h"
#include "client.h"

class RosNode : public Server {
    private:
        mutex nodelock;
        Node node;
        NodeCallBack _sf;
        ClientCallBack _cf;
        int master_port;
        char *master_ip;
    public:
        void CreateServer();
        void WaitForConnect();
        
        void Reg(int, char *, int, char *, string);
        string RegT();
        void Sub(string);
        void Pub(string);
        void Data(string, vector<int>);
        void Data(string, int);
        bool FindPublist(string);

        void CreateClient(string);

        static void ServerHandler(int *, struct sockaddr_in *, RosNode *);

        RosNode(int port, char *ip, NodeCallBack sf, ClientCallBack cf) : 
        Server(port, ip), _sf(sf), _cf(cf){};
        ~RosNode();
};

string RosNode::RegT() {
    return REG + "[name:" + node.name + ";ip:" + node.ip + ";port:" + to_string(node.port) + ";]";
}
void RosNode::Reg(int port, char *ip, int mport, char *mip, string name) {
    node.ip = ip;
    node.name = name;
    node.port = port;
    master_port = mport;
    master_ip = mip;
    CreateClient(RegT());
}
void RosNode::Sub(string s) {
    nodelock.try_lock();
    node.sub_list.push_back(s);
    CreateClient(SUB+"[name:"+node.name+";sub:"+s+";]");
    nodelock.unlock();
}
void RosNode::Pub(string s) {
    nodelock.try_lock();
    node.pub_list.push_back(s);
    CreateClient(PUB+"[name:"+node.name+";pub:"+s+";]");
    nodelock.unlock();
}
void RosNode::Data(string pubname, vector<int> x) {
    string s;
    for (auto i : x)
    {
        s += to_string(i) + ',';
    }
    if (FindPublist(pubname)) CreateClient(DATA+"[name:"+node.name+";pub:"+pubname+";"+s+"]");
    else cout << "error pubname..." << endl;
}
void RosNode::Data(string pubname, int x) {
    if (FindPublist(pubname))
        CreateClient(DATA+"[name:"+node.name+";pub:"+pubname+";"+to_string(x)+",]");
    else
        cout << "error pubname..." << endl;
}
bool RosNode::FindPublist(string pubname) {
    for (int i = 0; i < node.pub_list.size(); i++)
    {
        if (node.pub_list[i] == pubname) return true;
    }
    return false;
}
void RosNode::CreateClient(string text) {
    Client client(master_port, master_ip, _cf, text);
    client.ClientInit();
    client.ClientBindIpAndPort();
}

void RosNode::CreateServer() {
    this->ServerInit();
    this->ServerBindIpAndPort();
}

void RosNode::WaitForConnect() {
    listen(socket_fd, 30);
    socklen_t len = sizeof(client);
    while (1) {
        int fd = accept(socket_fd,(struct sockaddr*)&client,&len);
        if (fd == -1)
        {
            cout << "accept错误\n" << endl;
            exit(-1);
        }
        thread t1(_sf, &fd, &client, this);
        t1.detach();
    }
}

void RosNode::ServerHandler(int *fd, struct sockaddr_in *client, RosNode *m) {
    char *ip = inet_ntoa(client->sin_addr);
    cout << "客户： 【" << ip << "】连接成功" << endl;
    write(*fd, "welcome", 7);
    char buffer[255]={};
    int size = read(*fd, buffer, sizeof(buffer));    
    cout << "接收到字节数为： " << size << endl;
    cout << "内容： " << buffer << endl;
    string name = buffer;
    /* rewrite */
    while
    (
        !(read(*fd, buffer, sizeof(buffer)) == 0 
        || read(*fd, buffer, sizeof(buffer)) == -1)
    );
    cout << "END" << endl;
    close(*fd);
}

void shit(RosNode *m) {
    m->CreateServer();
    m->WaitForConnect();
}

void StartServer(RosNode *m) {
    thread t(shit, m);
    t.detach();
}

RosNode::~RosNode() {
    cout << "Node stop!\n";
}