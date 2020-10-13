#include "server.h"
#include "client.h"

void MyServerCallBack(int *fd, struct sockaddr_in *client, Master *MT) {
    /* rewrite */

    /* rewrite */
}

void MyClientCallBack(int *socket_fd) {
    string text = "name:gyh;port:8889;ip:0.0.0.0";
    int len = text.size();
    char *c = new char[text.length()+1];
    strcpy(c, text.c_str());
    write(*socket_fd, c, len);

    char buffer[255]={};
    int size = read(*socket_fd, buffer, sizeof(buffer));

    cout << "接收到字节数为： " << size << endl;
    cout << "内容： " << buffer << endl;
}

void CreateServer(Server *s) {
    s->ServerInit();
    s->ServerBindIpAndPort();
    s->WaitForConnect();
}

int main()
{
    int port = 8888;
    string ip = "127.0.0.1";
    Client client(port, ip, MyClientCallBack);
    client.ClientInit();
    client.ClientBindIpAndPort();
    while (keepRunning);
    
    return 0;
}