#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <csignal>
#include <vector>

using namespace std;

class Server;
class Client;
class MyThread;
class Master;

static int keepRunning = 1;

void stest(int *, struct sockaddr_in *);
typedef decltype(stest)* ServerCallBack;

void ctest(int *);
typedef decltype(ctest)* ClientCallBack;

static void SigThread(int);
void SigThread(int sig) {
    if (sig == SIGINT || sig == SIGSTOP)
    {
        keepRunning = 0;
        cout << endl;
    }
}

struct Node {
    int port;
    string ip;
    string name;
    vector<string> sub_list;
    vector<string> put_list;
};