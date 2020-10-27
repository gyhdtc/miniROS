#include "include/server.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, Master *m) {
    /* rewrite */
    char *ip = inet_ntoa(client->sin_addr);
    char endflag[2] = {'#', '*'};
    char buffer[255]={};
    int dataflag = 0;
    int size = 0;
    while(dataflag == 0)
    {
        size = read(*fd, buffer, sizeof(buffer));
        if (size == -1 || size == 0) break;
        if (DEBUG) cout << "*1" << endl;
        cout << buffer << endl;
    }
    /* rewrite */
    close(*fd);
}

void MyClientCallBack(int *socket_fd, string s) {
    /* rewrite */
    char flag[1] = {'\0'};
    int x = 0;
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
    int port = 8888;
    string ip = "0.0.0.0";
    

    signal(SIGINT, SigThread);
    while (keepRunning);
    return 0;
}