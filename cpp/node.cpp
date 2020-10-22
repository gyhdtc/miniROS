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
    if (DEBUG) cout << "5" << endl;
    char flag[1] = {'\0'};
    int x = 0;
    while (*flag != '#')
    {
        if (DEBUG) cout << "#1" << endl;
        char *t = new char(s.length()+1);
        strcpy(t, s.c_str());
        if (DEBUG) cout << "#2" << t << endl;
        write(*socket_fd, t, s.length());
        if (DEBUG) cout << "#3" << endl;
        read(*socket_fd, flag, 1);
        if (DEBUG) cout << "#4" << flag << endl;
    }
    if (DEBUG) cout << "6" << endl;
    /* rewrite */
}

int main()
{
    string name = "gyh1";
    int port = 8889;
    char *ip = (char *)"0.0.0.0";
    RosNode node1(port, ip, MyServerCallBack, MyClientCallBack);
    StartServer(&node1);
    
    int master_port = 8888;
    char *master_ip = (char *)"127.0.0.1";

    node1.Reg(port, ip, master_port, master_ip, name);
    node1.Pub("blue1");
    for (int i = 0; i < 5; i++) 
    {
        for (int j = 0; j < 200; j++)
            node1.Data("blue1", i*200+j);
    }
    signal(SIGINT, SigThread);

    while (keepRunning);
    return 0;
}