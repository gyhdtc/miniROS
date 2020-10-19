#ifndef A
#define A
#include "header.h"
#endif
#include "server.h"
#include "client.h"

class Master : public Server {
    public:
        map<string, int> name2index;
        vector<Node> nodes;
        vector<MessageQueue> MQ;
        ServerCallBack _sf;
        ClientCallBack _cf;
    public:
        void PushNode(string, string, int);
        void AddSub(string, string);
        void AddPub(string, string);
        void GetData(string, string, vector<int>);
        void ShowMQ();
        void ShowNodes();
        
        void CreateServer();
        void WaitForConnect();
        void CreateClient(char *, int, string);

        static void ServerHandler(int *, struct sockaddr_in *, Master *);

        Master(int port, char *ip, ServerCallBack cb, ClientCallBack cf) : Server(port, ip), _sf(cb), _cf(cf) {};
        Master(int port, string ip, ServerCallBack cb, ClientCallBack cf) : Server(port, ip), _sf(cb), _cf(cf) {};
        ~Master();
};

void Master::PushNode(string nodename, string ip, int port) {
    if (name2index.count(nodename) != 0)
    {
        cout << "exsist node" << endl;
        return;
    }
    Node n;
    n.name = nodename;
    n.ip = ip;
    n.port = port;
    n.index = nodes.size();
    nodes.push_back(n);
    name2index[n.name] = n.index;
    cout << "+++add a new node+++" << n.name << endl;
    return;
}
void Master::AddSub(string nodename, string subname) {
    cout << "收：" << nodename << " " << subname << endl;
    int flag = 0;
    int index = name2index.count(nodename) > 0 ? name2index[nodename]: -1;
    if (index == -1)
    {
        cout << "no node " << nodename << endl;
        return;
    }
    for (auto i : nodes[index].sub_list)
    {
        if (i == subname)
        {
            cout << "xxx exsist subname of " << subname << endl;
            return;
        }
    }
    for (int i = 0; i < MQ.size(); i++)
    {
        if (MQ[i].name == subname)
        {
            MQ[i].subnodelist.push_back(index);
            flag  = 1;
            break;
        }
    }
    if (flag != 1)
    {
        MessageQueue mq;
        mq.name = subname;
        mq.subnodelist.push_back(index);
        MQ.push_back(mq);
    }
    nodes[index].sub_list.push_back(subname);
    cout << "+++add a dingyue " << subname << " to " << nodename << endl;
    return;
}
void Master::AddPub(string nodename, string pubname) {
    int flag = 0;
    int index = name2index.count(nodename) > 0 ? name2index[nodename]: -1;
    if (index == -1)
    {
        cout << "no node " << nodename << endl;
        return;
    }
    for (auto i : nodes[index].pub_list)
    {
        if (i == pubname)
        {
            cout << "xxx exsist pubname of " << pubname << endl;
            return;
        }
    }
    for (int i = 0; i < MQ.size(); i++)
    {
        if (MQ[i].name == pubname)
        {
            flag = 1;
            if (MQ[i].pubnode != -1)
            {
                cout << "xxx exsist pubname in nodename " << MQ[i].name << endl;
                return;
            }
            else
            {
                MQ[i].pubnode = index;
                return;
            }
            
        }
    }
    if (flag == 0)
    {
        MessageQueue mq;
        mq.name = pubname;
        mq.pubnode = index;
        MQ.push_back(mq);
    }
    nodes[index].pub_list.push_back(pubname);
    cout << "+++add a fabu " << pubname << " to " << nodename << endl;
    return;
}
void Master::GetData(string nodename, string pubname, vector<int> s) {
    int index;
    for (int i = 0; i < MQ.size(); i++)
    {
        if (nodes[MQ[i].pubnode].name == nodename && MQ[i].name == pubname && MQ[i].savedataflag == false)
        {
            MQ[i].savedataflag = true;
            index = i;
            break;
        }
    }
    for (int i = 0; i < s.size(); i++)
    {
        MQ[index].data.push(s[i]);
    }
    MQ[index].savedataflag = false;
}
void Master::ShowMQ() {
    cout << "---------------------------" << endl;
    for (auto i : MQ)
    {
        cout << "消息[" << i.name << "]" << endl;
        
        cout << "发布节点：";
        if (i.pubnode != -1)
            cout << nodes[i.pubnode].name << endl;
        else
            cout << "---" << endl;
        
        cout << "订阅节点：";
        for (auto& j : i.subnodelist)
        {
            cout << nodes[j].name << " ";
        }
        cout << endl;

        cout << "数据：" << endl;
        int myqueue_size = i.data.size();
        for(int k = 0; k < myqueue_size; k++)
        {
            cout << i.data.front() << " ";
            i.data.push(i.data.front());
            i.data.pop();
        }
        cout << endl;
    }
    cout << "---------------------------" << endl;
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

void Master::CreateClient(char * ip, int port, string text) {
    Client client(port, ip, _cf, text);
    client.ClientInit();
    client.ClientBindIpAndPort();
}
void shit(Master *m) {
    m->CreateServer();
    m->WaitForConnect();
}
void shit3(Master *m, int index) {
    while(!m->MQ[index].data.empty())
    {
        int x = m->MQ[index].data.front();
        m->MQ[index].data.pop();
        for (int i = 0; i < m->MQ[index].subnodelist.size(); i++)
        {
            int nodeindex = m->MQ[index].subnodelist[i];
            int port = m->nodes[nodeindex].port;
            string ip = m->nodes[nodeindex].ip;
            char *c = new char[ip.size()+1];
            mystrncpy(c, ip.c_str(), ip.size());
            m->CreateClient(c, port, to_string(x));
            cout << x << endl;
            delete []c;
        }
    }
    m->MQ[index].flag = false;
}

void shit2(Master *m) {
    while (1)
    {
        for (int i = 0; i < m->MQ.size(); i++)
        {
            if (m->MQ[i].flag != true && !m->MQ[i].data.empty() && m->MQ[i].subnodelist.size() != 0)
            {
                m->MQ[i].flag = true;
                thread t(shit3, m, i);
                t.detach();
            }
        }
    }
    
}

void StartServer(Master *m) {
    thread t1(shit, m);
    t1.detach();
    thread t2(shit2, m);
    t2.detach();
}

Master::~Master() {
    cout << "master stop!\n";
}