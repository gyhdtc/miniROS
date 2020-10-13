#include "server.h"
#include "client.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, Master *MT) {
    /* rewrite */

    /* rewrite */
}
void CreateServer(Server *s) {
    s->ServerInit();
    s->ServerBindIpAndPort();
    s->WaitForConnect();
}

int main()
{
    int port = 8888;
    string ip = "0.0.0.0";
    Master master;
    Server s(&master);
    thread t(CreateServer, &s);
    t.detach();
    while (keepRunning) {
        sleep(1);
        cout << master.GetNameLen() << endl;
    };
    return 0;
}