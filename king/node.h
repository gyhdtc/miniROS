#ifndef A
#define A
#include "header.h"
#endif
#include "server.h"
#include "client.h"

class RosNode : public Server {
    private:
        Node node;
        NodeCallBack _sf;
        ClientCallBack _cf;
        int master_port;
        char *master_ip;
    public:
        void sub(string);
        void put(string);

        void CreateServer();
        void WaitForConnect();
        
        void zhuce(int, char *, int, char *, string);
        string zhuce_t();

        void sub(string);

        void CreateClient(int, char *, ClientCallBack, string);

        static void ServerHandler(int *, struct sockaddr_in *, RosNode *);

        RosNode(int port, char *ip, NodeCallBack sf, ClientCallBack cf) : 
        Server(port, ip), _sf(sf), _cf(cf){};
        ~RosNode();
};  
string RosNode::zhuce_t() {
    return "[name:" + node.name + ";ip:" + node.ip + ";port:" + to_string(node.port) + "]";
}
void RosNode::zhuce(int port, char *ip, int mport, char *mip, string name) {
    node.ip = ip;
    node.name = name;
    node.port = port;
    master_port = mport;
    master_ip = mip;
    CreateClient(master_port, master_ip, _cf, zhuce_t());
}

void RosNode::CreateClient(int port, char *ip, ClientCallBack cb, string text) {
    Client client(port, ip, cb, text);
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