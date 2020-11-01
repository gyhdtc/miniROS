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
        /* ----------按照格式发送------- */
        void Reg(int, char *, int, char *, string); // 注册
        string RegT(); 
        void Sub(string); // 订阅
        void Pub(string); // 发布
        void Data(string, vector<int>); // 发送消息
        void Data(string, int); // 发送消息
        bool FindPublist(string); // 判断是否有发布此消息
        /* --------按照格式发送--------- */

        void CreateServer(); // 创建服务器线程，keeprunning
        void WaitForConnect(); // 创建服务器线程，keeprunning
        void CreateClient(string); // 创建客户端，发送 string；创建线程，发送完，就销毁

        // 构造函数
        RosNode(int port, char *ip, NodeCallBack sf, ClientCallBack cf) : Server(port, ip), _sf(sf), _cf(cf){};
        // 析构函数
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
    if (FindPublist(pubname))
        CreateClient(DATA+"[name:"+node.name+";pub:"+pubname+";"+s+"]");
    else
        cout << "error pubname..." << endl;
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
        int fd = accept(socket_fd, (struct sockaddr*)&client, &len);
        if (fd == -1)
        {
            cout << errno << endl;
            cout << "accept错误\n" << endl;
            exit(-1);
            //continue;
        }
        /* header.h : nodeparam */
        struct NodeParam *sp;
        sp->fd = &fd;
        sp->client = &client;
        sp->n = this;
        /* header.h : nodeparam */
        thread t1(_sf, sp);
        t1.detach();
    }
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