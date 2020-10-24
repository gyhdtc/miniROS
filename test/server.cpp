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
#include <algorithm>
using namespace std;

class Server {
    public:
        int _port;
        char *_ip;
        int socket_fd;
        struct sockaddr_in addr;
        struct sockaddr_in client;

        void ServerInit();
        void ServerBindIpAndPort();
        void WaitForConnect();
        static void ServerHandler(int *, struct sockaddr_in *);
        Server(int port, char *ip) : _port(port), _ip(ip) {};
        ~Server();
};

void Server::ServerInit() {
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
        thread t1(ServerHandler, &fd, &client);
        t1.detach();
    }
}

Server::~Server() {
    close(socket_fd);
    cout << "close server" << endl;
}

void Server::ServerHandler(int *fd, struct sockaddr_in *client) {
    /* rewrite */
    char *ip = inet_ntoa(client->sin_addr);
    cout << "客户： 【" << ip << "】连接成功" << endl;
    int a = 12345;
    string s = to_string(a);
    char *t = new char[s.size()+1];
    strcpy(t, s.c_str());
    t[s.size()] = '\0';
    write(*fd, t, s.size());
    write(*fd, "welcome", 7);
    write(*fd, "#", 1);
    char buffer[1]={};

    int size = read(*fd, buffer, sizeof(buffer));    
    cout << "接收到字节数为： " << size << endl;
    cout << "内容： " << buffer << endl;
    /* rewrite */

    while
    (
        !(read(*fd, buffer, sizeof(buffer)) == 0 
        || read(*fd, buffer, sizeof(buffer)) == -1)
    );
    
    cout << "END" << endl;
    close(*fd);
}
int main()
{
    int port = 8888;
    char * ip = (char *)"0.0.0.0";
    Server server(port, ip);
    server.ServerInit();
    server.ServerBindIpAndPort();
    server.WaitForConnect();
    while (1);
    return 0;
}