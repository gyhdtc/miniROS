#include "include/node.h"

void MyNodeServerCallBack(void *param) {
    /* header.h : clientparam */
    NodeParam *sp = (NodeParam *)param;
    /* rewrite */
    char *ip = inet_ntoa(sp->client->sin_addr);
    char buffer[100]={};
    int size = 0;
    while (!((size = read(*sp->fd, buffer, sizeof(buffer))) <= 0))
    {
        cout << "内容： " << buffer << endl;
    }
    close(*sp->fd);
    /* rewrite */
    delete sp;
}

void MyClientCallBack(void *param) {
    /* header.h : clientparam */
    ClientParam *cp = (ClientParam *)param;
    /* rewrite */
    int len = cp->s.length();
    int x = 0;
    char *t = new char[len+1];
    strcpy(t, cp->s.c_str());
    write(cp->socket_fd, t, len);
    close(cp->socket_fd);
    delete []t;
    /* rewrite */
    delete cp;
}

int main()
{
    string name = "gyh1";
    int port = 8889;
    int master_port = 8888;
    char *ip = (char *)"0.0.0.0";
    char *master_ip = (char *)"127.0.0.1";
    // char *ip = (char *)"49.123.118.159";
    // char *master_ip = (char *)"115.157.195.140";
    
    RosNode node1(port, ip, MyNodeServerCallBack, MyClientCallBack);
    StartServer(&node1);

    node1.Reg(port, ip, master_port, master_ip, name);
    node1.Pub("blue1");

    // test send data
    for (int i = 0; i < 500; i++) 
    {
        node1.Data("blue1", i);
    }
    // test send data

    signal(SIGINT, SigThread); // 读取信号量
    while (keepRunning);
    return 0;
}