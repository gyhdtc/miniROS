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
#include <map>
#define DEBUG 0
#define REG to_string(1)
#define SUB to_string(2)
#define PUB to_string(3)
using namespace std;

class Master;
class Server;
class Client;
class MyThread;
class RosNode;

static int keepRunning = 1;

void stest(int *, struct sockaddr_in *, Master *);
typedef decltype(stest)* ServerCallBack;
void ntest(int *, struct sockaddr_in *, RosNode *);
typedef decltype(ntest)* NodeCallBack;
void ctest(int *, string);
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
    int index;
    int port;
    string ip;
    string name;
    vector<string> sub_list;
    vector<string> pub_list;
};

struct MessageQueue {
    int pubnode = -1;
    string name;
    vector<int> subnodelist;
};