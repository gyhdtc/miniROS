#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
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
#define DEBUG 1
#define DEBUG2 1
#define REG to_string(1)
#define SUB to_string(2)
#define PUB to_string(3)
#define DATA to_string(4)
#define MAX_SIZE_MQ 100
#define MAX_SIZE_NODE 100
using namespace std;

class Master;
class Server;
class Client;
class MyThread;
class RosNode;

static int keepRunning = 1;

void CallBackfun(void *param);
typedef decltype(CallBackfun)* ServerCallBack;
typedef decltype(CallBackfun)* NodeCallBack;
typedef decltype(CallBackfun)* ClientCallBack;

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

/* header.h : serverparam */
struct ServerParam {
    int *fd;
    struct sockaddr_in *client;
    Master *m;
};
/* header.h : clientparam */
struct ClientParam {
    int socket_fd;
    string s;
};
/* header.h : nodeparam */
struct NodeParam {
    int *fd;
    struct sockaddr_in *client;
    RosNode *n;
};

char * mystrncpy(char *dest, const char *src, int n) {
    strncpy(dest, src, n);
    dest[n] = '\0';
    return dest;
}

void GetIpAddress()
{
    struct ifaddrs * ifAddrStruct = NULL;
    struct ifaddrs * ifa = NULL;
    void * tmpAddrPtr = NULL;
    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
        {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        }
        else if (ifa->ifa_addr->sa_family == AF_INET6)
        {   
            tmpAddrPtr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        }
    }
    printf("---------------------------\n"); 
}