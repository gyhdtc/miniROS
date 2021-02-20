#ifndef A
#define A
#include "header.h"
#endif
#include "server.h"
#include "client.h"

void transmitData(Master *);
void sendThread(Master *, int);
void serverThread(Master *);
void StartServer(Master *);

class Master : public Server {
    public:
        mutex mqlock;
        mutex nodelock;
        int num_mq = 0;
        int num_node = 0;
        map<string, int> name2index;
        vector<Node> nodes;
        vector<MessageQueue> MQ;
        ServerCallBack _sf;
        ClientCallBack _cf;
    public:
        void AddNode(string, string, int);
        void AddSub(string, string);
        void AddPub(string, string);
        void GetData(string, string, vector<int>);
        void ShowMQ();
        void ShowNodes();
        
        void CreateServer();
        void WaitForConnect();
        void CreateClient(char *, int, string);

        void StartServer();
        void TransmitData();
        static void serverThread(Master *);
        static void sendThread(Master *, int);

        Master(int port, char *ip, ServerCallBack cb, ClientCallBack cf) : Server(port, ip), _sf(cb), _cf(cf) {};
        Master(int port, string ip, ServerCallBack cb, ClientCallBack cf) : Server(port, ip), _sf(cb), _cf(cf) {};
        ~Master();
};

void Master::AddNode(string nodename, string ip, int port) {
    nodelock.try_lock();
    if (name2index.count(nodename) != 0)
    {
        cout << "exsist node" << endl;
        return;
    }
    nodes[num_node].name = nodename;
    nodes[num_node].ip = ip;
    nodes[num_node].port = port;
    nodes[num_node].index = num_node;
    name2index[nodename] = num_node;
    num_node++;
    if (DEBUG) printf("+++ add a node [%s][%s][%d]\n", nodename.c_str(), ip.c_str(), port);
    nodelock.unlock();
}

void Master::AddSub(string nodename, string subname) {
    int flag = 0;
    int index = name2index.count(nodename) > 0 ? name2index[nodename]: -1;
    if (index == -1)
    {
        cout << "no node " << nodename << endl;
        return;
    } else
    {
        nodelock.try_lock();
        mqlock.try_lock();
    }
    
    for (auto i : nodes[index].sub_list)
    {
        if (i == subname)
        {
            cout << "exsist subname of " << subname << endl;
            nodelock.unlock();
            mqlock.unlock();
            return;
        }
    }
    for (int i = 0; i < num_mq; i++)
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
        MQ[num_mq].name = subname;
        MQ[num_mq].subnodelist.push_back(index);
        num_mq++;
    }
    nodes[index].sub_list.push_back(subname);
    if (DEBUG) printf("+++ add a sub [%s] to [%s]\n", subname.c_str(), nodename.c_str());
    nodelock.unlock();
    mqlock.unlock();
}

void Master::AddPub(string nodename, string pubname) {
    int flag = 0;
    int index = name2index.count(nodename) > 0 ? name2index[nodename]: -1;
    if (index == -1)
    {
        cout << "no node " << nodename << endl;
        return;
    } else
    {
        nodelock.try_lock();
        mqlock.try_lock();
    }

    for (auto i : nodes[index].pub_list)
    {
        if (i == pubname)
        {
            cout << "exsist pubname of " << pubname << endl;
            nodelock.unlock();
            mqlock.unlock();
            return;
        }
    }
    for (int i = 0; i < num_mq; i++)
    {
        if (MQ[i].name == pubname)
        {
            flag = 1;
            if (MQ[i].pubnode != -1)
            {
                cout << "exsist pubname in nodename " << MQ[i].name << endl;
                nodelock.unlock();
                mqlock.unlock();
                return;
            }
            else
            {
                MQ[i].pubnode = index;
                nodes[index].pub_list.push_back(pubname);
                nodelock.unlock();
                mqlock.unlock();
                return;
            }
        }
    }
    if (flag == 0)
    {
        MQ[num_mq].name = pubname;
        MQ[num_mq].pubnode = index;
        num_mq++;
    }
    nodes[index].pub_list.push_back(pubname);
    if (DEBUG) printf("+++ add a pub [%s] to [%s]\n", pubname.c_str(), nodename.c_str());
    nodelock.unlock();
    mqlock.unlock();
}

void Master::GetData(string nodename, string pubname, vector<int> s) {
    int index;
    for (int i = 0; i < num_mq; i++)
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
        if (DEBUG) printf("+++ add a data [%d] to [%s]\n", s[i], nodename.c_str());
        MQ[index].data.push(s[i]);
        MQ[index].num ++;
    }
    MQ[index].savedataflag = false;
}

void Master::ShowMQ() {
    cout << "-----------" << num_mq << "----------------" << endl;
    for (int i = 0; i < num_mq; i++)
    {
        cout << "消息[" << MQ[i].name << "]" << endl;
        
        cout << "发布节点：";
        if (MQ[i].pubnode != -1)
            cout << nodes[MQ[i].pubnode].name << endl;
        else
            cout << "---" << endl;
        
        cout << "订阅节点：";
        for (auto& j : MQ[i].subnodelist)
        {
            cout << nodes[j].name << " ";
        }
        cout << endl;

        cout << "(" << MQ[i].num << ")" << "数据：" << endl;
        int myqueue_size = MQ[i].data.size();
        for(int k = 0; k < myqueue_size; k++)
        {
            cout << MQ[i].data.front() << " ";
            MQ[i].data.push(MQ[i].data.front());
            MQ[i].data.pop();
        }
        cout << endl;
    }
    cout << "-----------" << num_mq << "----------------" << endl;
}

void Master::ShowNodes() {
    cout << "---------------------------" << endl;
    for (int i = 0; i < num_node; i++)
    {
        cout << "节点[" << nodes[i].name << "]" << endl;
        cout << "[" << nodes[i].ip << " : " << nodes[i].port << "]" << endl;
        cout << "发布消息：";
        for (auto j : nodes[i].pub_list)
        {
            cout << j << " ";
        }
        cout << endl;
        cout << "订阅消息：";
        for (auto j : nodes[i].sub_list)
        {
            cout << j << " ";
        }
        cout << endl;
    }
    cout << "---------------------------" << endl;
}

void Master::CreateServer() {
    this->ServerInit();
    this->ServerBindIpAndPort();
}

void Master::WaitForConnect() {
    /* learn */
    listen(socket_fd, 300);
    /*
    -- 参数2：socket可排队最大连接个数
    */
    socklen_t len = sizeof(client);
    while (1) {
        /* learn */
        int fd = accept(socket_fd, (struct sockaddr*)&client, &len);
        /*
        -- 已连接的socket描述字
        */
        if (fd == -1)
        {
            cout << errno << endl;
            cout << "accept错误\n" << endl;
            exit(-1);
            //continue;
        }
        /* header.h : serverparam */
        ServerParam sp;
        sp.fd = fd;
        sp.client = client;
        sp.m = this;
        /* header.h : serverparam */
        thread t1(_sf, sp);
        t1.detach();
    }
}

void Master::CreateClient(char * ip, int port, string s) {
    Client client(port, ip, _cf, s);
    client.ClientInit();
    client.ClientBindIpAndPort();
}

Master::~Master() {
    cout << "master stop!\n";
}

void Master::TransmitData() {
    for (int i = 0; i < this->MQ.size(); i++)
    {
        if (this->MQ[i].flag == false && !this->MQ[i].data.empty() && this->MQ[i].subnodelist.size() != 0)
        {
            this->MQ[i].flag = true;
            thread t(Master::sendThread, this, i);
            t.detach();
        }
    }
}

void realsendthread(Master *m, string ip, int port, string text, int *send_num_flag, mutex *a) {
    char *cip = new char[ip.size()+1];
    mystrncpy(cip, ip.c_str(), ip.size());
    m->CreateClient(cip, port, text);
    // rewrite
    // 并行发送 data，可尝试更优雅的方法
    a->lock();
    (*send_num_flag) ++;
    a->unlock();
    // 并行发送 data，可尝试更优雅的方法
    delete []cip;
}

void Master::sendThread(Master *m, int index) {
    while(!m->MQ[index].data.empty())
    {
        int x = m->MQ[index].data.front();
        int send_size = m->MQ[index].subnodelist.size();
        string text = "[" + m->MQ[index].name + ":" + to_string(x) + "]";
        // 并行发送 data，可尝试更优雅的方法
        int send_num_flag = 0;
        mutex a;
        // 并行发送 data，可尝试更优雅的方法
        for (int i = 0; i < send_size; i++)
        {
            int nodeindex = m->MQ[index].subnodelist[i];
            int port = m->nodes[nodeindex].port;
            string ip = m->nodes[nodeindex].ip;
            // rewrite
            thread t1(realsendthread, m, ip, port, text, &send_num_flag, &a);
            t1.detach();
        }
        // rewrite
        // 并行发送 data，可尝试更优雅的方法
        while (send_num_flag < send_size);
        // 并行发送 data，可尝试更优雅的方法
        m->MQ[index].data.pop();
    }
    m->MQ[index].flag = false;
}

void Master::serverThread(Master *m) {
    m->CreateServer();
    m->WaitForConnect();
}

void Master::StartServer() {
    this->MQ.resize(MAX_SIZE_MQ);
    this->nodes.resize(MAX_SIZE_NODE);
    thread t1(Master::serverThread, this);
    t1.detach();
}