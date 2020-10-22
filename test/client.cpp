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

static volatile int keepRunning = 1; 

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

int main()
{
    int cport = 8888;
    string cip = "127.0.0.1";
    while(1) {
        Client cshit(cport, cip);
        cshit.Init();
        cshit.BindIpAndPort();
        sleep(3);
    }
    
    //while (keepRunning);
    
    return 0;
}