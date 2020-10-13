#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <csignal>

using namespace std;

class Server;
class MyThread;

static volatile int keepRunning = 1; 

class Server {
    private:
        int _port;
        char *_ip;
        int socket_fd;
        struct sockaddr_in addr;
        struct sockaddr_in client;
    public:
        void Init();
        void BindIpAndPort();
        void WaitForConnect();
        bool LegitimateIp(const char *);
        Server(int port, char *ip) : _port(port), _ip(ip) {};
        Server(int, string);
        Server();
        ~Server();
};
class Client {
    private:
        int _port;
        char *_ip;
        int socket_fd;
        struct sockaddr_in addr;
    public:
        void Init();
        void BindIpAndPort();
        static void ClientHandler(int *);
        Client(int port, char *ip) : _port(port), _ip(ip) {};
        Client(int, string);
        Client();
        ~Client();
};

class MyThread {
    public:
        void CreateServer(Server *);
        static void ServerThread(Server *);
        void CreateSig();
        static void SigThread(int);
        ~MyThread();
};

void ConnectHandler(int *fd, struct sockaddr_in *client) {
    char *ip = inet_ntoa(client->sin_addr);
    cout << "客户： 【" << ip << "】连接成功" << endl;

    write(*fd, "welcome", 7);

    char buffer[255]={};

    int size = read(*fd, buffer, sizeof(buffer));    
    cout << "接收到字节数为： " << size << endl;
    cout << "内容： " << buffer << endl;

    while(
        !(read(*fd, buffer, sizeof(buffer)) == 0 
        || read(*fd, buffer, sizeof(buffer)) == -1)
    );
    cout << "END" << endl;

    close(*fd);
}

void Server::Init() {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        cout << "socket 创建失败： "<< endl;
        exit(1);
    }
    addr.sin_family = AF_INET;
    if (_port > 7000 && _port < 9000)
    {
        addr.sin_port = htons(_port);
    }
    if (LegitimateIp(_ip))
    {
        addr.sin_addr.s_addr = inet_addr(_ip);
    }
}

void Server::BindIpAndPort() {
    int res = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1)
    {
        cout << "bind创建失败： " << endl;
        exit(-1);
    }
    cout << "bind ok 等待客户端的连接" << endl;
}

void Server::WaitForConnect() {
    listen(socket_fd,30);
    socklen_t len = sizeof(client);
    while (1) {
        int fd = accept(socket_fd,(struct sockaddr*)&client,&len);
        if (fd == -1)
        {
            cout << "accept错误\n" << endl;
            exit(-1);
        }
        thread t1(ConnectHandler, &fd, &client);
        t1.detach();
    }
}

bool Server::LegitimateIp(const char *s) {
    return true;
}

Server::Server() {
    this->_port = 8888;
    this->_ip = (char*)"127.0.0.1";
}

Server::Server(int port, string s) {
    this->_port = port;
    char *ip = new char[s.length()+1];
    strcpy(ip, s.c_str());
    this->_ip = ip;
}

Server::~Server() {
    close(socket_fd);
    cout << "close server" << endl;
}

void Client::Init() {
    socket_fd = socket(AF_INET, SOCK_STREAM,0);
    if(socket_fd == -1)
    {
        cout<<"socket 创建失败："<<endl;
        exit(-1);
    }
    addr.sin_family = AF_INET;
    if (_port > 7000 && _port < 9000)
    {
        addr.sin_port = htons(_port);
    }
    addr.sin_addr.s_addr = inet_addr(_ip);
}

void Client::BindIpAndPort() {
    cout << addr.sin_port << endl;
    int res = connect(socket_fd,(struct sockaddr*)&addr,sizeof(addr));
    if(res == -1)
    {
        cout<<"bind 链接失败："<<endl;
        //exit(-1);
        return;
    }
    cout<<"bind 链接成功："<<endl;
    thread t(ClientHandler, &this->socket_fd);
    t.detach();
}

void Client::ClientHandler(int *socket_fd) {
    write(*socket_fd,"hello hebinbing",15);

    char buffer[255]={};
    int size = read(*socket_fd, buffer, sizeof(buffer));

    cout << "接收到字节数为： " << size << endl;
    cout << "内容： " << buffer << endl;
}

Client::Client() {
    this->_port = 8888;
    this->_ip = (char*)"127.0.0.1";
}

Client::Client(int port, string s) {
    this->_port = port;
    char *ip = new char[s.length()+1];
    strcpy(ip, s.c_str());
    this->_ip = ip;
}

Client::~Client() {
    close(socket_fd);
    cout << "close client" << endl;
}

void MyThread::CreateServer(Server *s) {
    thread t(ServerThread, s);
    t.detach();
    cout << "Create Server Thread" << endl;
}

void MyThread::ServerThread(Server *s) {
    s->Init();
    s->BindIpAndPort();
    s->WaitForConnect();
}

void MyThread::CreateSig() {
    signal(SIGINT, SigThread);
    cout << "Create Sig Thread" << endl;
}

void MyThread::SigThread(int sig) {
    if (sig == SIGINT || sig == SIGSTOP)
    {
        keepRunning = 0;
        cout << endl;
    }
}

MyThread::~MyThread() {
    cout << "close mythread" << endl;
}

int main()
{
    int port = 8890;
    string ip = "0.0.0.0";
    
    Server shit(port, ip);
    MyThread gyh;
    gyh.CreateServer(&shit);
    gyh.CreateSig();

    int cport = 8888;
    string cip = "127.0.0.1";
    Client cshit1(cport, cip);
    cshit1.Init();
    cshit1.BindIpAndPort();
    sleep(3);

    int cport2 = 8889;
    string cip2 = "127.0.0.1";
    Client cshit2(cport2, cip2);
    cshit2.Init();
    cshit2.BindIpAndPort();
    sleep(3);

    while (keepRunning);
    
    return 0;
}