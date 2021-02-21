#ifndef A
#define A
#include "header.h"
#endif

class Client {
    public:
        int _port;
        char *_ip;
        //Master *m;
        //int index;
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
    /* learn6 */
    int res = connect(socket_fd,(struct sockaddr*)&addr,sizeof(addr));
    /*
    -- 从 进程 到 内核
    -- 参数不介绍了，learn3,learn5 说的很详细。
    -- 介绍下此函数与三次握手的关系：
    -- 调用connect，客户端发送第一次 SYN；服务器接受到之后，返回SYN和ACK，connect接收到之后立即返回；
    -- 可能会发生的错误1：
    -- “硬错误” 服务器没有进程在监听此端口，服务器返回RST，connect接收到之后立即返回错误，econnrefuse；
    -- “软错误” 目标不可到达，不管是路由返回的ICMP还是本机发现目标不可到达，都会进行重连（等待一段时间），返回 ***unreach错误；
    -- “超时“ 一直无响应
    */
    if(res == -1)
    {
        cout << errno << endl;
        cout<<"client bind false" << endl;
        return;
    }
    /* header.h : clientparam */
    ClientParam cp;
    // cp = new ClientParam;
    cp.socket_fd = socket_fd;
    cp.s = text;
    /* header.h : clientparam */
    usleep(200*((rand()%50+100)));
    thread t(_cf, cp);
    t.join();
    // 这里会与 clientcallback 中的 delete 冲突
    // 故意留一个bug，学一下调试 strace
    // delete cp;
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
}