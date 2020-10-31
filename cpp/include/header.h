#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <csignal>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <errno.h>
#define DEBUG 0
#define DEBUG2 1
#define REG to_string(1)
#define SUB to_string(2)
#define PUB to_string(3)
#define DATA to_string(4)
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
    vector<string> sub_list;// 订阅
    vector<string> pub_list;// 发布
};

struct MessageQueue {
    bool flag = false;
    bool savedataflag = false;
    int pubnode = -1;
    int num = 0;
    string name;
    vector<int> subnodelist;
    queue<int> data;
};

char * mystrncpy(char *dest, const char *src, int n) {
    strncpy(dest, src, n);
    dest[n] = '\0';
    return dest;
}