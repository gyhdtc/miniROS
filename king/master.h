#ifndef A
#define A
#include "header.h"
#endif
#include "server.h"

class Master : public Server {
    public:
        vector<Node> nodes;
        vector<MessageQueue> MQ;
        ServerCallBack _sf;
    public:
        void PushNode(string, string, int);
        void AddSub(string, string);
        void AddPub(string, string);
        void ShowMQ();
        void ShowNodes();
        void ShowOneNode(Node *);
        
        void CreateServer();
        void WaitForConnect();
        
        static void ServerHandler(int *, struct sockaddr_in *, Master *);

        Master(int port, char *ip, ServerCallBack cb) : Server(port, ip), _sf(cb) {};
        Master(int port, string ip, ServerCallBack cb) : Server(port, ip), _sf(cb) {};
        ~Master();
};

void Master::PushNode(string nodename, string ip, int port) {
    cout << "---begin add a new node---" << endl;
    for (auto i : nodes)
    {
        if (i.name == nodename)
        {
            cout << "node exsist" << endl;
            return;
        }
        else if(i.ip == ip && i.port == port)
        {
            cout << "ip:port exsist" << endl;
            return;           
        }
    }
    Node n;
    n.name = nodename;
    n.ip = ip;
    n.port = port;
    nodes.push_back(n);
    cout << "+++add a new node+++" << n.name << endl;
    return;
}
void Master::AddSub(string nodename, string subname) {
    cout << "---begin add a new sub---" << endl;
    int flag = 0;
    int index; 
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i].name == nodename)
        {
            index = i;
            break;
        }
    }
    for (auto i : nodes[index].sub_list)
    {
        if (i == subname)
        {
            cout << "xxxexsist subname of " << subname << endl;
            return;
        }
    }
    for (auto i : MQ)
    {
        if (i.name == subname)
        {
            i.subnodelist.push_back(&nodes[index]);
            flag  = 1;
            break;
        }
    }
    if (flag != 1)
    {
        MessageQueue mq;
        mq.name = subname;
        mq.subnodelist.push_back(&nodes[index]);
        MQ.push_back(mq);
    }
    nodes[index].sub_list.push_back(subname);
    //ShowOneNode(t);
    cout << "+++add a dingyue " << subname << " to " << nodename << endl;
    return;
}
void Master::AddPub(string nodename, string pubname) {
    cout << "---begin add a new pub---" << endl;
    int flag = 0;
    int index; 
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i].name == nodename)
        {
            index = i;
            break;
        }
    }
    for (auto i : MQ)
    {
        if (i.name == pubname)
        {
            if (i.pubnode != nullptr)
            {
                flag = -1;
                cout << "xxxpubname " << pubname << " exsist" << endl;
                return;
            }
            else
            {
                i.pubnode = &nodes[index];
                flag = 1;
            }
            break;
        }
    }
    if (flag == 0)
    {
        MessageQueue mq;
        mq.pubnode = &nodes[index];
        mq.name = pubname;
        MQ.push_back(mq);
    }
    nodes[index].pub_list.push_back(pubname);
    cout << "+++add a fabu " << pubname << " to " << nodename << endl;
    return;
}

void Master::ShowMQ() {
    for (auto i : MQ)
    {
        cout << "消息[" << i.name << "]" << endl;
        cout << "发布节点：";
        if (i.pubnode != nullptr)
            cout << i.pubnode->name << endl;
        else
            cout << "---" << endl;
        cout << "订阅节点：";
        for (auto j : i.subnodelist)
        {
            cout << j->name << " ";
        }
        cout << endl;
    }
}

void Master::ShowNodes() {
    for (auto i : nodes)
    {
        cout << "节点[" << i.name << "]" << endl;
        cout << "[" << i.ip << " : " << i.port << "]" << endl;
        cout << "发布消息：";
        for (auto j : i.pub_list)
        {
            cout << j << " ";
        }
        cout << endl;
        cout << "订阅消息：";
        for (auto j : i.sub_list)
        {
            cout << j << " ";
        }
        cout << endl;
    }
}

void Master::ShowOneNode(Node *i) {
    cout << "[" << i->name << "]" << endl;
    cout << "[" << i->ip << " : " << i->port << "]" << endl;
    cout << i->pub_list.size() << endl;
    for (auto j : i->pub_list)
    {
        cout << j << endl;
    }
    for (auto j : i->sub_list)
    {
        cout << j << endl;
    }
}

void Master::CreateServer() {
    this->ServerInit();
    this->ServerBindIpAndPort();
}

void Master::WaitForConnect() {
    listen(socket_fd,30);
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

void Master::ServerHandler(int *fd, struct sockaddr_in *client, Master *m) {
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

void shit(Master *m) {
    m->CreateServer();
    m->WaitForConnect();
}

void StartServer(Master *m) {
    thread t(shit, m);
    t.detach();
}

Master::~Master() {
    cout << "master stop!\n";
}