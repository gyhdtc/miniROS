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
#include <signal.h>
#include <string>
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

class Client;
class Master;

struct Data {
    uint32_t DataType;
    string data;
};
struct Topic
{
    uint8_t FaBuNode;
    uint32_t DingYueNode;
    string TopicName;
};
struct Head
{
    msg_type type;
    uint8_t check_code;
    uint8_t machine_num;
    uint8_t data_len;
    uint8_t rand_num;
    uint32_t data_type;
};