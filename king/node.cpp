#include "node.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, RosNode *m) {
    cout << "this is my server fun" << endl;
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

void MyClientCallBack(int *socket_fd, string s) {
    /* rewrite */
    char *t = new char(s.length()+1);
    strcpy(t, s.c_str());
    write(*socket_fd, t, s.length());
    /* rewrite */
}

int main()
{
    string name = "gyh1";
    int port = 8889;
    char *ip = (char *)"0.0.0.0";
    RosNode node1(port, ip, MyServerCallBack, MyClientCallBack);
    StartServer(&node1);
    cout << "zhuce";
    int master_port = 8888;
    char *master_ip = (char *)"127.0.0.1";
    node1.zhuce(port, ip, master_port, master_ip, name);

    signal(SIGINT, SigThread);

    while (keepRunning);
    return 0;
}