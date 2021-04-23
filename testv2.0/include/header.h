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
#include <memory>
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
using namespace std;

int KeepRunning = 1;

typedef int State;
#define state_close 0
#define state_connect 1
#define state_disconnect 2

typedef uint8_t msg_type;
#define heart 0x01
#define ack 0x02
#define reg 0x04
#define sub 0x08
#define pub 0x10
#define dat 0x20
#define headlength 10
typedef uint8_t data_type;
#define MAX_BUFFER_SIZE 265

typedef vector<uint8_t> msg_packet;

class Client;
class Master;
class Data;
class Head;
struct Topic;

msg_packet zhuce(uint8_t, string);
void DataGenera(Data&, string);
void MsgHandle(Head, shared_ptr<char>);

class Data {
public:
    vector<uint8_t> DataType;
    vector<char> charlist;
    vector<uint32_t> intlist;
    vector<double> doublelist;
    vector<uint8_t> databytestream;
};
struct Topic
{
    uint8_t FaBuNode;
    uint32_t DingYueNode;
    string TopicName;
};
class Head
{
public:
    int type;
    int check_code;
    int machine_num;
    int data_len;
    int rand_num;
    int datatype;
    int index;
};

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
// 数据类型生成器，统计data中各个列表的个数
void DataTypeGenera(Data& d) {
    d.DataType.resize(4);

    if (!d.charlist.empty()) {
        d.DataType[0] |= 0x00000001;
        d.DataType[1] = uint8_t(d.charlist.size());
    }
    if (!d.intlist.empty()) {
        d.DataType[0] |= 0x00000002;
        d.DataType[2] = uint8_t(d.intlist.size());
    }
    if (!d.doublelist.empty()) {
        d.DataType[0] |= 0x00000004;
        d.DataType[3] = uint8_t(d.doublelist.size());
    }
}
// 拼接vector
void Combine(vector<uint8_t>& a, const vector<uint8_t>& b) {
    for (int i = 0; i < b.size(); ++i) {
        a.push_back(b[i]);
    }
}
// 8位比特位输出
void out(uint8_t* a, size_t len) {
    cout << "---------------------\n";
    for (int i = 0; i < len; ++i) {
        cout << bitset<8>(uint8_t(*(a+i))) << endl;
    }
    cout << "---------------------\n";
}
// 头部解析
void GetHead(Head& h, const void* start) {
    uint8_t* t = (uint8_t*)start;
    h.type = uint8_t(*(t++));
    h.check_code = uint8_t(*(t++));
    h.machine_num = uint8_t(*(t++));
    h.data_len = uint8_t(*(t++));
    h.rand_num = uint8_t(*(t++));
    int i = 4;
    h.datatype = uint32_t(0);
    while (i--) {
        h.datatype |= ((uint32_t(*(t++))) << (8*(3-i)));
    }
    h.index = uint8_t(*t);
    // cout << (h.type) << endl;
    // cout << (h.check_code) << endl;
    // cout << (h.machine_num) << endl;
    // cout << (h.data_len) << endl;
    // cout << (h.rand_num) << endl;
    // cout << (h.datatype) << endl;
    // cout << (h.index) << endl;
}
// 头部生成器
vector<uint8_t> HeadGenera(uint32_t index, Data data, msg_type msgtype) {
    vector<uint8_t> headbytestream(5,0x00);
    
    uint8_t machine_num = index == 0 ? 0 : 1;
    while (index > 1) {
        machine_num *= 2;
        index >> 1;
    }
    headbytestream[0] = msgtype;
    headbytestream[1] = codeGenera(data.databytestream);
    headbytestream[2] = machine_num;
    headbytestream[3] = data.databytestream.size();
    srand((int)time(0));
    headbytestream[4] = rand()%256;

    return headbytestream;
}
void SigThread(int sig) {
    if (sig == SIGINT || sig == SIGSTOP)
    {
        KeepRunning = 0;
        cout << endl;
    }
}