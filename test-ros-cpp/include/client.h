#ifndef A
#define A
#include "header.h"
#endif

class Client {
    public:
        int _port;
        char *_ip;
        ClientCallBack _cf; 
        string text;
        int socket_fd;
        struct sockaddr_in addr;

        void ClientInit();
        void ClientBindIpAndPort();

        static void ClientHandler(int *, string);

        Client(int port, char *ip, ClientCallBack cb, string t) : _port(port), _ip(ip), _cf(cb), text(t) {};
        ~Client();
};

void Client::ClientInit() {
    cout << "4" << endl;
    socket_fd = socket(AF_INET, SOCK_STREAM,0);
    if(socket_fd == -1)
    {
        cout<<"socket create false" << endl;
    }
    addr.sin_family = AF_INET;
    if (_port > 7000 && _port < 9000)
    {
        addr.sin_port = htons(_port);
    }
    addr.sin_addr.s_addr = inet_addr(_ip);
}

void Client::ClientBindIpAndPort() {
    cout << "5" << endl;
    int res = connect(socket_fd,(struct sockaddr*)&addr,sizeof(addr));
    if(res == -1)
    {
        cout<<"client bind false" << endl;
        return;
    }
    cout<<"client bind success" << endl;
    //usleep(1000*(rand()%10));
    thread t(_cf, &this->socket_fd, text);
    t.join();
}

void Client::ClientHandler(int *socket_fd, string s) {
    write(*socket_fd,"hello ",15);
    char buffer[255]={};
    int size = read(*socket_fd, buffer, sizeof(buffer));
    cout << "接收到字节数为： " << size << endl;
    cout << "内容： " << buffer << endl;
}

Client::~Client() {
    close(socket_fd);
    cout << "close client" << endl;
}