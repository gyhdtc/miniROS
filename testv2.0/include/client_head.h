#ifndef CLIENTHEAD
#define CLIENTHEAD
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <errno.h>
#include <queue>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <memory.h>
#include <signal.h>
#include <bitset>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <atomic>
#include <set>
#include <assert.h>
#include <semaphore.h>
using namespace std;
#define int32_t uint32_t
// 信号
int KeepRunning = 1;
// 定义常量
#define IPADDRESS   "127.0.0.1"
#define PORT        8787
#define MAXSIZE     1024
#define LISTENQ     100
#define headlength  8
#define MAX_BUFFER_SIZE  263
// 定义 node 状态
#define _newconnect 1
#define _connecting 2
#define _connected 3
#define _connect_wait 4
#define _close 5
#define _online 6
// 状态转换有向图
// 没有状态 0，第一行第一列全为 0
int state_transfer[7][7] = 
{
//      0,1,2,3,4,5,6
/* 0 */ 0,0,0,0,0,0,0,
/* 1 */ 0,1,1,0,1,0,0,
/* 2 */ 0,0,1,1,0,1,0,
/* 3 */ 0,0,0,1,0,1,1,
/* 4 */ 0,0,1,0,1,1,0,
/* 5 */ 0,0,0,0,0,1,0,
/* 6 */ 0,0,0,0,0,1,1
};
vector<string> DP({"", "newconnect", "connecting", "connected", "connect_wait", "close", "online"});
// 协议第一个字节的含义
const uint8_t heartbeat     = 0b00000001;
const uint8_t getheartbeat  = 0b00000010;
const uint8_t subtopic      = 0b00001000;
const uint8_t pubtopic      = 0b00000100;
const uint8_t deltopic      = 0b00001100;
const uint8_t regnode       = 0b00010000;
const uint8_t getregnode    = 0b00100000;
const uint8_t reconnect     = 0b01000000;
const uint8_t data          = 0b10000000;

struct Head {
    uint8_t type;
    uint8_t node_index;
    uint8_t topic_name_len;
    uint8_t data_len;
    uint8_t check_code;
    uint8_t return_node_index;
    uint16_t resever_int; // 保留字节
    Head() {
        type = 0x00;
        node_index = 0x00;
        topic_name_len = 0x00;
        data_len = 0x00;
        check_code = 0x00;
        return_node_index = 0x00;
        resever_int = 0x0000;
    }
};
struct Msg {
    Head head;
    shared_ptr<char> buffer;
    Msg() {
        buffer = NULL;
    }
};
// 头部解析
void GetHead(Head& h, const void* start) {
    uint8_t* t = (uint8_t*)start;
    h.type = uint8_t(*(t++));
    h.node_index = uint8_t(*(t++));
    h.topic_name_len = uint8_t(*(t++));
    h.data_len = uint8_t(*(t++));
    h.check_code = uint8_t(*(t++));
    h.return_node_index = uint8_t(*(t++));
    h.resever_int = uint16_t(*t);
}
// 8位比特位输出
void out(uint8_t* a, size_t len) {
    cout << "---------------------\n";
    for (int i = 0; i < len; ++i) {
        if (i < 8) {
            cout << bitset<8>(uint8_t(*(a+i))) << endl;
        }
        else {
            cout << *(a+i);
        }
    }
    if (len > 8) cout << endl;
    cout << "---------------------\n";
}
void out(char* a, size_t len) {
    cout << "---------------------\n";
    for (int i = 0; i < len; ++i) {
        if (i < 8) {
            cout << bitset<8>(uint8_t(*(a+i))) << endl;
        }
        else {
            cout << *(a+i);
        }
    }
    if (len > 8) cout << endl;
    cout << "---------------------\n";
}
void out(string s) {
    cout << "---------------------\n";
    for (int i = 0; i < s.size(); ++i) {
        if (i < 8) {
            cout << bitset<8>(uint8_t(*(s.c_str()+i))) << endl;
        }
        else {
            cout << *(s.c_str()+i);
        }
    }
    if (s.size() > 8) cout << endl;
    cout << "---------------------\n";
}
// 生成校验码：统计 1 的个数，和256取余
uint8_t codeGenera(uint8_t* msg, size_t len) {
    uint8_t res = 0;
    for (int i = 0; i < len; ++i) {
        uint8_t t = 0x01;
        for (int j = 0; j < 8; ++j) {
            if ((msg[i] & t) != 0x00) {
                res = res % 256 + 1;
            }
            t = t << 1;
        }
    }
    return res;
}
// 机器码转整数
uint8_t node_index2int8(uint32_t node_index) {
    assert(node_index);
    int i = 0;
    while (node_index) {
        i++;
        node_index = node_index >> 1;
    }
    assert(i <= 32);
    return (uint8_t)i;
}
// 整数转机器码
uint32_t int82node_index(uint8_t node_index) {
    node_index = (int)node_index;
    uint32_t res = 0x0000000001;
    while (--node_index) {
        res = res << 1;
    }
    return res;
}
// 生成校验码
uint8_t codeGenera(const char *Msg, int len) {
    uint8_t res = 0;
    for (int i = 0; i < len; ++i) {
        uint8_t t = 0x01;
        for (int j = 0; j < 8; ++j) {
            if (((*(Msg+i)) & t) != 0x00) {
                res = res % 256 + 1;
            }
            t = t << 1;
        }
    }
    return res;
}
// string转到const char*
void string2Msg(Msg msg, const string& topicname, const string& data) {
    assert(sizeof(msg.head) == 8);
    assert(msg.buffer != NULL);
    int len = 8 + topicname.size() + data.size();
    // shared_ptr<char> buffer(new char[len]);
    memcpy(msg.buffer.get(), &msg.head, 8);    
    for (int i = 0; i < topicname.size(); i++) {
        *(msg.buffer.get()+8+i) = topicname[i];
    }
    for (int i = 0; i < data.size(); i++) {
        *(msg.buffer.get()+8+topicname.size()+i) = data[i];
    }
}
void string2Msg(Msg msg, const string&& topicname, const string&& data) {
    assert(sizeof(msg.head) == 8);
    assert(msg.buffer != NULL);
    int len = 8 + topicname.size() + data.size();
    // shared_ptr<char> buffer(new char[len]);
    memcpy(msg.buffer.get(), &msg.head, 8);    
    for (int i = 0; i < topicname.size(); i++) {
        *(msg.buffer.get()+8+i) = topicname[i];
    }
    for (int i = 0; i < data.size(); i++) {
        *(msg.buffer.get()+8+topicname.size()+i) = data[i];
    }
}
// ctrl + c 的程序中止线程
void SigThread(int sig) {
    if (sig == SIGINT || sig == SIGSTOP)
    {
        KeepRunning = 0;
        cout << endl;
    }
}
#endif