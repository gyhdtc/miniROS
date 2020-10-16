#include "master.h"
#include "client.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, Master *m) {
    /* rewrite */
    char *ip = inet_ntoa(client->sin_addr);
    char buffer[255]={};
    int size = read(*fd, buffer, sizeof(buffer));    
    cout << buffer << endl;
    switch (buffer[0])
    {
    case '1':
    {
        string nodename, ip;
        int port;
        int flag = 0;
        int i, j;
        for (i = 0; i < size; i++)
        {
            if (buffer[i] == ':')
            {
                i += 1;
                for (j = i; j < size; j++)
                {
                    if (buffer[j] == ';')
                    {
                        flag ++;
                        char *c = new char[j-i];
                        strncpy(c, buffer+i, j-i);
                        if (flag == 1) nodename = (string)c;
                        if (flag == 2) ip = (string)c;
                        if (flag == 3) port = atoi(c);
                        delete []c;
                        break;
                    }                      
                }
            }
        }
        m->PushNode(nodename, ip, port);
        break;
    }
    case '2':
    {
        string nodename, subname;
        int flag = 0;
        int i, j;
        for (i = 0; i < size; i++)
        {
            if (buffer[i] == ':')
            {
                i += 1;
                for (j = i; j < size; j++)
                {
                    if (buffer[j] == ';')
                    {
                        flag ++;
                        char *c = new char[j-i];
                        strncpy(c, buffer+i, j-i);
                        if (flag == 1) nodename = (string)c;
                        if (flag == 2) subname = (string)c;
                        delete []c;
                        break;
                    }                      
                }
            }
        }
        m->AddSub(nodename, subname);
        break;
    }
    case '3':
    {
        string nodename, pubname;
        int flag = 0;
        int i, j;
        for (i = 0; i < size; i++)
        {
            if (buffer[i] == ':')
            {
                i += 1;
                for (j = i; j < size; j++)
                {
                    if (buffer[j] == ';')
                    {
                        flag ++;
                        char *c = new char[j-i];
                        strncpy(c, buffer+i, j-i);
                        if (flag == 1) nodename = (string)c;
                        if (flag == 2) pubname = (string)c;
                        delete []c;
                        break;
                    }                      
                }
            }
        }
        m->AddPub(nodename, pubname);
        break;
    }   
    default:
    {
        cout << "error data" << endl;
        break;
    }
    }
    /* rewrite */
    while
    (
        !(read(*fd, buffer, sizeof(buffer)) == 0 
        || read(*fd, buffer, sizeof(buffer)) == -1)
    );
    close(*fd);
}

void MyClientCallBack(int *socket_fd) {
    /* rewrite */

    /* rewrite */
}

int main()
{
    int port = 8888;
    string ip = "0.0.0.0";
    Master master(port, ip, MyServerCallBack);
    StartServer(&master);

    signal(SIGINT, SigThread);
    while (keepRunning)
    {
        sleep(5);
        //master.ShowNodes();
        //sleep(1);
        master.ShowMQ();
    }
    return 0;
}