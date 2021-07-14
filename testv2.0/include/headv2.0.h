#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <errno.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory.h>
#include <signal.h>
#include <bitset>
#include <stdarg.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <atomic>
#include <set>
using namespace std;
#define int32_t uint32_t
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
#define _close_1 6
// 状态转换有向图
// 没有状态 0，第一行第一列全为 0
int state_transfer[6][6] = 
{
//      0,1,2,3,4,5
/* 0 */ 0,0,0,0,0,0,
/* 1 */ 0,1,1,0,1,0,
/* 2 */ 0,0,1,1,0,1,
/* 3 */ 0,0,0,1,0,1,
/* 4 */ 0,0,1,0,1,1,
/* 5 */ 0,0,0,0,0,1
};
vector<string> DP({"", "newconnect", "connecting", "connected", "connect_wait", "close"});
/* 声明一些类 */
class Server;
class Node;
class Topic;
class Broke;

struct Head {
    uint8_t type;
    uint8_t node_index;
    uint8_t topic_name_len;
    uint8_t data_len;
    uint8_t check_code;
    uint8_t return_node_index;
    uint16_t resever_int; // 保留字节
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
        cout << bitset<8>(uint8_t(*(a+i))) << endl;
    }
    cout << "---------------------\n";
}
// 生成校验码：统计 1 的个数，和256取余
uint8_t codeGenera(vector<uint8_t> msg) {
    uint8_t res = 0;
    for (int i = 0; i < msg.size(); ++i) {
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