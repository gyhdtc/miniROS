#include "../include/clienthead.h"
// 连接server，管理线程
void ManageClient(MyNode* const);
class Client
{
private:
    int socketFd;
    struct sockaddr_in dest_addr;
    int serverPort;
    string serverIp;
public:
    bool ConnectBroke();
    Client(string ip, int port) : serverIp(ip), serverPort(port) {
        printf("Create a client , server = %s:%d\n", serverIp.c_str(), serverPort);
    }
};
bool Client::ConnectBroke() {    
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        printf("Error: socket\n");
        return false;
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(serverPort);
    dest_addr.sin_addr.s_addr = inet_addr(serverIp.c_str());
    if (connect(socketFd, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
        printf("Error : connect\n");
        return false;
    }
    return true;
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
class Node
{
private:
    // 状态
    int State;
    mutex StateLock;
    // 节点序号
    int32_t nodeIndex;
    // 数据传输后，不变
    string nodeName;
    // 节点 订阅/发布 的话题
    set<string> subTopicList;
    set<string> pubTopicList;
    // 数据消息
    deque<Msg> Message;
    mutex MsgLock;
    sem_t Msg_Sem;
public:
    // 设置节点
    int SetIndex(int32_t);
    // 节点状态设置
    int GetState();
    void SetState(int);
    // 等待节点关闭
    void WaitForClose();
    Node(string _name) : nodeName(_name) {
        State = _newconnect;
        printf("Create a node = %s\n", nodeName.c_str());
    }
};
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
class MyNode
{
private:
    Node* node;
    Client* client;
public:
    void StartClient();
    MyNode(string _name, string _ip, int _port) {
        client = new Client(_ip, _port);
        node = new Node(_name);
        printf("Start Client\n");
    }
};
void MyNode::StartClient() {
    if (client->ConnectBroke()) {
        printf("Success start client\n");
        thread t(ManageClient, this);
        t.join();
    }
    else {
        printf("fail start client\n");
    }
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
void ManageClient(MyNode* const mynode) {

}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
int main() {
    string myname = "gyh-1";
    string serverip = "127.0.0.1";
    int serverport = 8787;
    MyNode* mynode = new MyNode(myname, serverip, serverport);
    mynode->StartClient();
    return 0;
}