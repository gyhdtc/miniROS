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
    char *t = new char(s.length()+1);
    strcpy(t, s.c_str());
    write(*socket_fd, t, s.length());
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
    node1.Pub("blue");
    node1.Pub("blue2");

    vector<int> a = {1,2,3,4,5};
    node1.Data("blue", a);
    vector<int> b = {6,7,8,9,10};
    node1.Data("blue2", b);
    

    return 0;
}