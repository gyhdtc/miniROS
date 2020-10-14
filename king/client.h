#ifndef A
#define A
#include "header.h"
#endif

class Client {
    public:
        int _port;
        char *_ip;
        int socket_fd;
        ClientCallBack _cf; 
        struct sockaddr_in addr;

        void ClientInit();
        void ClientBindIpAndPort();

        static void ClientHandler(int *);

        Client(int port, char *ip, ClientCallBack cb) : _port(port), _ip(ip), _cf(cb) {};
        Client(int, string, ClientCallBack);
        Client();
        ~Client();
};

void Client::ClientInit() {
    signal(SIGINT, SigThread);
    socket_fd = socket(AF_INET, SOCK_STREAM,0);
    if(socket_fd == -1)
    {
        cout<<"socket 创建失败："<<endl;
    }
    addr.sin_family = AF_INET;
    if (_port > 7000 && _port < 9000)
    {
        addr.sin_port = htons(_port);
    }
    addr.sin_addr.s_addr = inet_addr(_ip);
}

void Client::ClientBindIpAndPort() {
    cout << addr.sin_port << endl;
    int res = connect(socket_fd,(struct sockaddr*)&addr,sizeof(addr));
    if(res == -1)
    {
        cout<<"bind 链接失败："<<endl;
        return;
    }
    cout<<"bind 链接成功："<<endl;
    thread t(_cf, &this->socket_fd);
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
    this->_cf = ClientHandler;
}

Client::Client(int port, string s, ClientCallBack cb) {
    this->_port = port;
    char *ip = new char[s.length()+1];
    strcpy(ip, s.c_str());
    this->_ip = ip;
    this->_cf = cb;
}

Client::~Client() {
    close(socket_fd);
    cout << "close client" << endl;
}