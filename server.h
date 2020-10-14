#ifndef A
#define A
#include "header.h"
#endif

class Server {
    private:
        int _port;
        char *_ip;
        int socket_fd;
        ServerCallBack _pf;
        struct sockaddr_in addr;
        struct sockaddr_in client;
        
    public:
        void ServerInit();
        void ServerBindIpAndPort();
        void WaitForConnect();
        static void ServerHandler(int *, struct sockaddr_in *);
        Server(int port, char *ip, ServerCallBack cb) : _port(port), _ip(ip), _pf(cb) {};
        Server(int, string, ServerCallBack);
        Server(ServerCallBack);
        Server();
        ~Server();
};

void Server::ServerInit() {
    signal(SIGINT, SigThread);
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
    addr.sin_addr.s_addr = inet_addr(_ip);
}

void Server::ServerBindIpAndPort() {
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
        thread t1(_pf, &fd, &client);
        t1.detach();
    }
}

Server::Server(int port, string s, ServerCallBack cb) {
    this->_port = port;
    
    char *ip = new char[s.length()+1];
    strcpy(ip, s.c_str());
    this->_ip = ip;

    this->_pf = cb;
}

Server::Server(ServerCallBack cb) {
    this->_port = 8888;
    this->_ip = (char*)"127.0.0.1";
    this->_pf = cb;
}

Server::Server() {
    this->_port = 8888;
    this->_ip = (char*)"127.0.0.1";
    this->_pf = ServerHandler;
}

Server::~Server() {
    close(socket_fd);
    cout << "close server" << endl;
}

void Server::ServerHandler(int *fd, struct sockaddr_in *client) {
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
    
    cout << "END" << endl;
    close(*fd);
}