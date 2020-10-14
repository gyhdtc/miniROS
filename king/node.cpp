#include "master.h"
#include "client.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, Master *m) {
    /* rewrite */
    char *ip = inet_ntoa(client->sin_addr);
    cout << "客户： 【" << ip << "】连接成功" << endl;
    write(*fd, "mycallback", 7);
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
    /* rewrite */
}

void MyClientCallBack(int *socket_fd) {
    /* rewrite */
    write(*socket_fd,"hello mycallback",16);
    char buffer[255]={};
    int size = read(*socket_fd, buffer, sizeof(buffer));
    cout << "接收到字节数为： " << size << endl;
    cout << "内容： " << buffer << endl;
    /* rewrite */
}

int main()
{
    int port = 8889;
    string ip = "0.0.0.0";
    Master master(port, ip, MyServerCallBack);
    StartServer(&master);

    Client client(8888, "127.0.0.1", MyClientCallBack);
    client.ClientInit();
    client.ClientBindIpAndPort();

    signal(SIGINT, SigThread);
    while (keepRunning);
    return 0;
}