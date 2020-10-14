#include "master.h"
#include "client.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, Master *m) {
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
    /* rewrite */
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
    while (keepRunning);
    return 0;
}