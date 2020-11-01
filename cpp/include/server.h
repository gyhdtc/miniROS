#ifndef A
#define A
#include "header.h"
#endif

class Server {
    public:
        int _port;
        char *_ip;
        int socket_fd;
        struct sockaddr_in addr;
        struct sockaddr_in client;

        void ServerInit();
        void ServerBindIpAndPort();
        
        Server(int port, char *ip) : _port(port), _ip(ip) {};
        Server(int, string);
        Server();
        ~Server();
};

void Server::ServerInit() {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        cout << "socket create false "<< endl;
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
        cout << "server bind false" << endl;
        exit(1);
    }
    cout << "server bind success" << endl;
}

Server::Server(int port, string s) {
    this->_port = port;
    char *ip = new char[s.length()+1];
    strcpy(ip, s.c_str());
    this->_ip = ip;
}

Server::Server() {
    this->_port = 8888;
    this->_ip = (char*)"0.0.0.0";
}

Server::~Server() {
    close(socket_fd);
    cout << "close server" << endl;
}