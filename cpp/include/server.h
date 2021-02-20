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
        
        Server(int port, char *ip) : _port(port), _ip(new char(*ip)) {};
        Server(int, string);
        Server();
        ~Server();
};

void Server::ServerInit() {
    /* learn */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    /*
    -- 创建socket文件描述符
    -- 参数1：协议簇，此处用了IP4，还有AF_INET6、AF_LOCAL、AF_ROUTE
    -- 参数2：socket类型，流格式套接字/数据报格式套接字 对应 SOCK_STREAM/SOCK_DGRAM
    --      上述两种类型工作在传输层；而SOCK_RAW,采用ip协议，工作在网络层
    -- 参数3：protocol，指定协议；当protocol为0时，会自动选择type类型对应的默认协议
    --      socket类型和协议不能随意组合！
    */
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
    /* learn */
    int res = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    /*
    -- 把一个ipv4或ipv6地址和端口号组合赋给socket
    -- 参数1：socket文件描述符
    -- 参数2：addr是一个结构体，里面描述了要绑定给socket文件描述符的协议族、ip和port
    --      没个协议族都不一样，上面 [36-41] 行描述的是ipv4的，ipv6的是sockaddr_in6：
            sa_family_t     sin6_family;    AF_INET6 
            in_port_t       sin6_port;      port number 
            uint32_t        sin6_flowinfo;  IPv6 flow information 
            struct in6_addr sin6_addr;      IPv6 address 
            uint32_t        sin6_scope_id;  Scope ID (new in 2.4)
    -- 参数3：上述结构体的长度
    -- 注意：通常服务器在启动的时候都会绑定一个众所周知的地址（如ip地址+端口号）
    --      用于提供服务客户就可以通过它来接连服务器
    --      而客户端就不用指定，有系统自动分配一个端口号和自身的ip地址组合。
    --      这就是为什么通常服务器端在listen之前会调用bind()
    --      而客户端就不会调用，而是在connect()时由系统随机生成一个。 
    */
    if (res == -1)
    {
        cout << "server bind false" << endl;
        exit(1);
    }
    cout << "server bind success : " << _ip << ":" << _port << endl;
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