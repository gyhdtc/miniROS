#include "include/node.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, RosNode *m) {
    /* rewrite */
    char *ip = inet_ntoa(client->sin_addr);
    char endflag[2] = {'#', '*'};
    char buffer[255]={};
    int dataflag = 0;
    int size = 0;
    while(dataflag == 0)
    {
        size = read(*fd, buffer, sizeof(buffer));
        if (size == -1 || size == 0) 
        {
            write(*fd, endflag+1, 1);
            continue;
        } else {
            cout << buffer << endl;
            write(*fd, endflag, 1);
            dataflag = 1;
        }
    }
    /* rewrite */
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
    string name = "gyh3";
    int port = 8891;
    // char *ip = (char *)"49.123.118.159";
    char *ip = (char *)"0.0.0.0";
    RosNode node1(port, ip, MyServerCallBack, MyClientCallBack);
    StartServer(&node1);
    
    int master_port = 8888;
    // char *master_ip = (char *)"115.157.195.140";
    char *master_ip = (char *)"127.0.0.1";

    node1.Reg(port, ip, master_port, master_ip, name);
    node1.Sub("blue1");
    
    signal(SIGINT, SigThread);

    while (keepRunning);
    return 0;
}