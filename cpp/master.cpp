#include "include/master.h"

void MyServerCallBack(void *param) {
    /* rewrite */
    /* header.h : serverparam */
    ServerParam *sp = (ServerParam *)param;
    char *ip = inet_ntoa(sp->client->sin_addr);
    char buffer[100];
    int dataflag = 0;
    int size = 0;
    while(!((size = read(*(sp->fd), buffer, sizeof(buffer))) <= 0))
    {
        if (DEBUG) cout << "*1 " << size << endl;
        cout << buffer << endl;
        if (!(buffer[0] <= '4' && buffer[0] >= '1') && buffer[size-1] != ']')
        {
            cout << "error data...reload..." << endl;
            break;
        }
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
                    for (j = i; j < size && buffer[j] != ']'; j++)
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
            sp->m->AddNode(nodename, ip, port);
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
                    for (j = i; j < size && buffer[j] != ']'; j++)
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
            sp->m->AddSub(nodename, subname);
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
                    for (j = i; j < size && buffer[j] != ']'; j++)
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
            sp->m->AddPub(nodename, pubname);
            break;
        }
        case '4':
        {
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
            for (; j < size && buffer[j] != ']'; j++)
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
            sp->m->GetData(nodename, pubname, s);
            break;
        }
        default:
        {
            cout << "error data...reload..." << endl;
            if (DEBUG) cout << "*2" << endl;
            break;
        }
        }
    }
    cout << "END" << endl;
    close(*(sp->fd));
    delete sp;
    /* rewrite */
}

void MyClientCallBack(void *param) {
    /* rewrite */
    /* header.h : clientparam */
    ClientParam *cp = (ClientParam *)param;
    int len = cp->s.length();
    int x = 0;
    char *t = new char[len+1];
    strcpy(t, cp->s.c_str());
    write(cp->socket_fd, t, len);
    close(cp->socket_fd);

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
            if (master.MQ[i].flag == false && !master.MQ[i].data.empty() && master.MQ[i].subnodelist.size() != 0)
            {
                master.MQ[i].flag = true;
                thread t(send_data, &master, i);
                t.detach();
            }
        }
    }
    master.ShowNodes();
    master.ShowMQ();
    return 0;
}