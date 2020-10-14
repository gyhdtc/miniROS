#include "master.h"
#include "client.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, Master *m) {
    /* rewrite */

    /* rewrite */
}

void MyClientCallBack(int *fd, struct sockaddr_in *client, Master *m) {
    /* rewrite */

    /* rewrite */
}

int main()
{
    int port = 8888;
    string ip = "0.0.0.0";
    Master master;

    StartServer(&master);
    signal(SIGINT, SigThread);
    
    while (keepRunning);
    return 0;
}