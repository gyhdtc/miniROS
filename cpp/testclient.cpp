#include "include/client.h"

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
        if (size == 0 || size == -1) 
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
    int x = 0;
    char *t = new char[s.length()+1];
    strcpy(t, s.c_str());    
    while (*flag != '#')
    {
        write(*socket_fd, t, s.length());
        read(*socket_fd, flag, 1);
    }
    delete []t;
    /* rewrite */
}

int main()
{
    string name = "gyh1";
    int port = 8889;
    // char *ip = (char *)"49.123.118.159";
    char *ip = (char *)"0.0.0.0";
    
    
    int master_port = 8888;
    // char *master_ip = (char *)"115.157.195.140";
    char *master_ip = (char *)"127.0.0.1";

    node1.Reg(port, ip, master_port, master_ip, name);
    node1.Pub("blue1");
    // test send data
    for (int i = 0; i < 200; i++) 
    {
        vector<int> a;
        for (int j = 0; j < 10; j++)
            a.push_back(i*10+j);
        node1.Data("blue1", a);
    }
    // test send data
    signal(SIGINT, SigThread);

    while (keepRunning);
    return 0;
}