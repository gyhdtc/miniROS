#ifndef A
#define A
#include "header.h"
#endif
#include "server.h"

class Master : public Server {
    private:
        vector<Node> nodes;
        ServerCallBack _sf;
        ClientCallBack _cf;
    public:
        void PushName(string);
        void GetName(int, string&);
        int GetNameLen();
        void CreateServer();
        void CreateSig();
        void WaitForConnect();
        
        static void ServerHandler(int *, struct sockaddr_in *, Master *);

        Master(int port, char *ip, ServerCallBack cb) : Server(port, ip), _sf(cb) {};
        Master(int port, string ip, ServerCallBack cb) : Server(port, ip), _sf(cb) {};
        Master(ServerCallBack cb) : Server(), _sf(cb) {};
        Master() : Server(), _sf(ServerHandler) {};
        ~Master();
};

void Master::PushName(string s) {
    Node t;
    t.name = s;
    nodes.push_back(t);
}

void Master::GetName(int i, string &name) {
    if (i > 0 && i <= nodes.size())
    {
        name = nodes[i-1].name;
        return;
    }
    name = "";
    return;
}

int Master::GetNameLen() {
    return nodes.size();
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
    /* rewrite */
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
    cout << m->nodes[0].name << endl;
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