#include "node.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, RosNode *m) {
    /* rewrite */
    char *ip = inet_ntoa(client->sin_addr);
    char buffer[255]={};
    int size = read(*fd, buffer, sizeof(buffer));    
    cout << "内容： " << buffer << endl;
    /* rewrite */
    while
    (
        !(read(*fd, buffer, sizeof(buffer)) == 0 
        || read(*fd, buffer, sizeof(buffer)) == -1)
    );
    close(*fd);
}

void MyClientCallBack(int *socket_fd, string s) {
    /* rewrite */
    char flag[1] = {'\0'};
    while (*flag != '#')
    {
        char *t = new char(s.length()+1);
        strcpy(t, s.c_str());
        write(*socket_fd, t, s.length());
        read(*socket_fd, flag, 1);
    }
    /* rewrite */
}

int main()
{
    string name = "gyh2";
    int port = 8890;
    char *ip = (char *)"0.0.0.0";
    RosNode node1(port, ip, MyServerCallBack, MyClientCallBack);
    StartServer(&node1);
    
    int master_port = 8888;
    char *master_ip = (char *)"127.0.0.1";

    node1.Reg(port, ip, master_port, master_ip, name);
    node1.Sub("blue1");
    
    signal(SIGINT, SigThread);

    while (keepRunning);
    return 0;
}