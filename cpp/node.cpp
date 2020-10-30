#include "include/node.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, RosNode *m) {
    /* rewrite */
    char *ip = inet_ntoa(client->sin_addr);
    char buffer[100]={};
    int size = 0;
    while (!((size = read(*fd, buffer, sizeof(buffer))) <= 0))
    {
        cout << "内容： " << buffer << endl;
    }
    /* rewrite */
}

void MyClientCallBack(int *socket_fd, string s) {
    /* rewrite */
    char flag[1] = {'\0'};
    int x = 0;
    char *t = new char[s.length()+1];
    strcpy(t, s.c_str());
    write(*socket_fd, t, s.length());
    close(*socket_fd);
    /* rewrite */
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
    
    RosNode node1(port, ip, MyServerCallBack, MyClientCallBack);
    StartServer(&node1); // --------------------
    
    node1.Reg(port, ip, master_port, master_ip, name);
    node1.Pub("blue1");

    // test send data
    for (int i = 0; i < 1000; i++) 
    {
        node1.Data("blue1", i);
    }
    // test send data

    signal(SIGINT, SigThread); // 读取信号量
    while (keepRunning);
    return 0;
}