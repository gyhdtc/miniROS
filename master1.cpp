#include "master.h"
#include "client.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client) {
    /* rewrite */

    /* rewrite */
}

int main()
{
    int port = 8888;
    string ip = "0.0.0.0";
    Master master(port, ip, MyServerCallBack);
    master.CreateServer();
    master.WaitForConnect();
    while (keepRunning) {
        sleep(1);
    };
    return 0;
}