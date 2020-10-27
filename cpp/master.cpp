#include "include/master.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, Master *m) {
    /* rewrite */
    char *ip = inet_ntoa(client->sin_addr);
    // char endflag[2] = {'#', '*'};
    char buffer[100]={};
    int dataflag = 0;
    int size = 0;
    while(dataflag == 0)
    {
        size = read(*fd, buffer, sizeof(buffer));
        if (size == -1 || size == 0) break;
        if (DEBUG) cout << "*1" << endl;
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
                            char *c = new char[j-i+1];
                            mystrncpy(c, buffer+i, j-i);
                            if (flag == 1) nodename = (string)c;
                            if (flag == 2) ip = (string)c;
                            if (flag == 3) port = atoi(c);
                            delete []c;
                            break;
                        }                      
                    }
                }
            }
            m->AddNode(nodename, ip, port);
            // write(*fd, endflag, 1);
            dataflag = 1;
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
                            char *c = new char[j-i+1];
                            mystrncpy(c, buffer+i, j-i); 
                            if (flag == 1) nodename = (string)c;
                            if (flag == 2) subname = (string)c;
                            
                            delete []c;
                            break;
                        }                      
                    }
                }
            }
            m->AddSub(nodename, subname);
            // write(*fd, endflag, 1);
            dataflag = 1;
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
                            char *c = new char[j-i+1];
                            mystrncpy(c, buffer+i, j-i);
                            if (flag == 1) nodename = (string)c;
                            if (flag == 2) pubname = (string)c;
                            delete []c;
                            break;
                        }                      
                    }
                }
            }
            m->AddPub(nodename, pubname);
            // write(*fd, endflag, 1);
            dataflag = 1;
            break;
        }
        case '4':
        {
            cout << "data" << endl;
            string nodename, pubname;
            vector<int> s;
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
                            char *c = new char[j-i+1];
                            mystrncpy(c, buffer+i, j-i);
                            if (flag == 1) nodename = (string)c;
                            if (flag == 2) pubname = (string)c;
                            delete []c;
                            break;
                        }                      
                    }
                }
            }
            i = j + 1;
            for (; j < size; j++)
            {
                if (buffer[j] == ',')
                {
                    char *d = new char[j-i+1];
                    mystrncpy(d, buffer+i, j-i);
                    s.push_back(atoi(d));
                    delete []d;
                    i = j + 1;
                }
            }
            m->GetData(nodename, pubname, s);
            // write(*fd, endflag, 1);
            dataflag = 1;
            break;
        }
        default:
        {
            cout << "error data...reload..." << endl;
            // write(*fd, endflag+1, 1);
            dataflag = 0;
            if (DEBUG) cout << "*2" << endl;
            break;
        }
        }
    }
    /* rewrite */
    close(*fd);
}

void MyClientCallBack(int *socket_fd, string s) {
    /* rewrite */
    char flag[1] = {'\0'};
    int x = 0;
    //while (*flag != '#')
    //{
        char *t = new char[s.length()+1];
        strcpy(t, s.c_str());
        write(*socket_fd, t, s.length());
        //read(*socket_fd, flag, 1);
    //}
    /* rewrite */
}

int main()
{
    int port = 8888;
    string ip = "0.0.0.0";
    Master master(port, ip, MyServerCallBack, MyClientCallBack);
    StartServer(&master);

    signal(SIGINT, SigThread);
    while (keepRunning)
    {
        for (int i = 0; i < master.MQ.size(); i++)
        {
            if (master.MQ[i].savedataflag != true && master.MQ[i].flag != true && !master.MQ[i].data.empty() && master.MQ[i].subnodelist.size() != 0)
            {
                master.MQ[i].flag = true;
                thread t(senddata, &master, i);
                t.detach();
            }
        }
    }
    master.ShowNodes();
    master.ShowMQ();
    return 0;
}