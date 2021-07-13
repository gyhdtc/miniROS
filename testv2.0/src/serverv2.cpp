#include "./include/headv2.0.h"

/* 声明一些线程函数 */
// 等待连接线程
void AccpetThread(Broke* const);
// 处理/管理 新连接
void ConnectThread(Broke* const, int, sockaddr_in);
// 新节点的 读/写 线程
void ReadThread(Broke* const, Node* const);
void WriteThread(Broke* const, Node* const);

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
class Server
{
private:
    int listenfd;
    int myport;
    string myIp;
public:
    void Bind();
    void Listen();
    int Accept(struct sockaddr_in&);
    Server(string ip, int port)
    {
        myIp = ip;
        myport = port;   
        printf("Create a server = %s:%d\n", myIp.c_str(), myport);
    }
    ~Server()
    {
        close(listenfd);
    }
};
void Server::Bind() {
    const char * ip = myIp.c_str();
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket error\n");
        exit(1);
    }
    printf("Create a listenfd , fd : %d\n", listenfd);
    // 清空结构体
    memset(&serveraddr, 0, sizeof(serveraddr));
    // 给结构体赋值，转换格式
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(myport);
    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind error\n");
        exit(1);
    }
    printf("bind success\n");
}
void Server::Listen() {
    listen(listenfd, LISTENQ);
}
int Server::Accept(struct sockaddr_in& cliaddr) {
    int clifd;
    socklen_t cliaddrlen = sizeof(cliaddr);
    clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
    return clifd;
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
class Node
{
private:
    int State;
    mutex StateLock;
    condition_variable CloseCv;

    int32_t nodeIndex;
    bool IndexFlag;
    bool TimeOutflag;
    mutex IndexLock;
    condition_variable IndexResetCv;

    string nodeIp;
    int nodePort;
    int connectfd;

    string nodeName;
    
    vector<string> subTopicList; // 节点订阅的话题
    vector<string> pubTopicList; // 节点发布的话题
public:
    int32_t GetIndex();
    bool ResetIndex(int);
    int SetIndex(int32_t);
    int GetState();
    void SetState(int);
    void WaitForClose();
    Node(string ip, int port, int fd)
    {
        nodeIp = ip;
        nodePort = port;
        connectfd = fd;
        nodeIndex = 0x00000000;
        State = _newconnect;
        IndexFlag = false;
        TimeOutflag = false;
        printf("Create a node = %s:%d\n", nodeIp.c_str(), nodePort);
    }
    ~Node()
    {
        printf("Close a Node = %s:%d\n", nodeIp.c_str(), nodePort);
        close(connectfd);
    }
};
int Node::SetIndex(int32_t index) {
    unique_lock<mutex> lk(IndexLock);
    if (TimeOutflag == true) {
        return -1;
    }
    else {
        if (IndexFlag == true) {
            return 0;
        }
        else {
            nodeIndex = index;
            IndexFlag = true;
            if (State == _connect_wait)
                IndexResetCv.notify_one();
            return 1;
        }
    }
}
bool Node::ResetIndex(int timeout = 3) {
    unique_lock<mutex> lk(IndexLock);
    printf("Re-Set Index = %s:%d\n", nodeIp.c_str(), nodePort);
    IndexResetCv.wait_for(lk, chrono::seconds(timeout), [this](){ return IndexFlag == true; });
    if (IndexFlag == false)
        TimeOutflag = true;
    return IndexFlag;
}
int32_t Node::GetIndex() {
    unique_lock<mutex> lk(IndexLock);
    return nodeIndex;
}
void Node::SetState(int transferstate) {
    unique_lock<mutex> lk(StateLock);
    if (state_transfer[State][transferstate] == 1) {
        State = transferstate;
        printf("Set Node-%d State to %s\n", nodeIndex, DP[transferstate].c_str());
        if (State == _close) CloseCv.notify_one();
        return;
    }
    return;
}
int Node::GetState() {
    unique_lock<mutex> lk(StateLock);
    return State;
}
void Node::WaitForClose() {
    unique_lock<mutex> lk(StateLock);
    CloseCv.wait(lk, [this](){ return State == _close; });
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
class Topic
{
private:
    map<int32_t, set<string>> sub2name; // 发布节点序号 --- 发布话题名称s
    map<int32_t, set<string>> pub2name; // 发布节点序号 --- 订阅话题名称s
    map<string, pair<int32_t, int32_t>> name2subANDpub; // 名字 --- （订阅节点，发布节点）
public:
    void subTopic(string, int32_t);
    void pubTopic(string, int32_t);
    void delTopic(string, int32_t); 
};
void Topic::subTopic(string name, int32_t index) {
    if (name2subANDpub.find(name) == name2subANDpub.end() || (name2subANDpub.find(name) != name2subANDpub.end() && name2subANDpub[name].first == 0)) {
        // 没有这个话题，或者有这个话题但是没有话题发布者
        if (name2subANDpub.find(name) == name2subANDpub.end()) {
            // 没有这个话题
            name2subANDpub[name] = {index, 0};
        }
        else {
            // 有这个话题
            name2subANDpub[name].first = index;
        }
        if (sub2name.find(index) == sub2name.end()) {
            sub2name.insert(pair<int32_t, set<string>>{index, set<string>{name}});
        }
        else {
            sub2name[index].insert(name);
        }
    }
    else {
        // 有这个话题，并且有话题发布者
        printf("%s already subed by %d\n", name.c_str(), index);
    }
}
void Topic::pubTopic(string name, int32_t index) {
    if (name2subANDpub.find(name) == name2subANDpub.end() || (name2subANDpub.find(name) != name2subANDpub.end() && (name2subANDpub[name].second & index) == 0)) {
        // 没有这个话题，或者有这个话题但是此节点没有订阅此话题
        if (name2subANDpub.find(name) == name2subANDpub.end()) {
            // 没有这个话题
            name2subANDpub[name] = {0, index};
        }
        else {
            // 有这个话题
            name2subANDpub[name].second = name2subANDpub[name].second | index;
        }
        if (pub2name.find(index) == pub2name.end()) {
            pub2name.insert(pair<int32_t, set<string>>{index, set<string>{name}});
        }
        else {
            pub2name[index].insert(name);
        }
    }
    else {
        // 有这个话题，并且有话题发布者
        printf("%s already subed by %d\n", name.c_str(), index);
    }
}
void Topic::delTopic(string name, int32_t index) {
    // 删除全部
    if (name == "all") {
        vector<string> tname;
        if (sub2name.find(index) != sub2name.end()) {
            for (auto i : sub2name[index]) {
                tname.push_back(i);
            }
            sub2name.erase(index);
        }
        if (pub2name.find(index) != pub2name.end()) {
            for (auto i : pub2name[index]) {
                tname.push_back(i);
            }
            pub2name.erase(index);
        }
        for (auto topicname : tname) {
            name2subANDpub[topicname].first &= (0xffffffff ^ index);
            name2subANDpub[topicname].second &= (0xffffffff ^ index);
            if (name2subANDpub[topicname].first == 0 && name2subANDpub[topicname].second == 0) {
                name2subANDpub.erase(topicname);
            }
        }
    }
    // 删除某一个
    else {
        if (sub2name[index].find(name) != sub2name[index].end()) {
            sub2name[index].erase(name);
        }
        if (pub2name[index].find(name) != pub2name[index].end()) {
            pub2name[index].erase(name);
        }
        if (name2subANDpub.find(name) != name2subANDpub.end()) {
            name2subANDpub[name].first &= (0xffffffff ^ index);
            name2subANDpub[name].second &= (0xffffffff ^ index);
        }
    }
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
class Broke
{
private:
    int32_t totalNode;
    mutex IndexLock;
    Server* server;
    Topic* topic;
    map<int32_t, Node*> index2nodeptr;
    mutex Index2NodeptrLock;
public:
    void StartServer(string, int);
    int WaitAccpet(struct sockaddr_in&);
    int32_t AllocIndex();
    bool AddNode(Node* const);
    bool DelNode(Node* const);
    Broke()
    {
        totalNode = 0;
        topic = new Topic;
        server = NULL;
        printf("Start broke\n");
    }
    ~Broke()
    {
        for (auto i : index2nodeptr) delete i.second;
        delete topic;
        delete server;
        printf("Stop broke\n");
    }
};
void Broke::StartServer(string ip, int port) {
    if (server == NULL) {
        server = new Server(ip, port);
    }
    server->Bind();
    server->Listen();
    AccpetThread(this);
}
int Broke::WaitAccpet(struct sockaddr_in& cliaddr) {
    int connectfd = server->Accept(cliaddr);
    return connectfd;
}
int32_t Broke::AllocIndex() {
    lock_guard<mutex> guard(IndexLock);
    int32_t t = 0x00000001;
    while ((t != 0) && ((totalNode & t) != 0x00000000)) t = t << 1;
    totalNode = totalNode | t;
    return t;
}
bool Broke::AddNode(Node* const node) {
    int32_t node_index = node->GetIndex();
    unique_lock<mutex> lk(Index2NodeptrLock);
    if (index2nodeptr.find(node_index) == index2nodeptr.end()) {
        index2nodeptr.insert({node_index, node});
        return true;
    }
    else {
        printf("Index %d exsit\n", node_index);
        return false;
    }
}
bool Broke::DelNode(Node* const node) {
    // 先删索引，再恢复 totalnode
    // index 不一定存在，先判断
    // 还要删除Topic下，对应的？【有没有简单点的方法？】
    unique_lock<mutex> guard1(IndexLock);
    unique_lock<mutex> guard2(Index2NodeptrLock);
    int32_t node_index = node->GetIndex();
    // 删除节点下对应的topic
    topic->delTopic("all", node_index);
    // 恢复节点序号
    totalNode = totalNode & (0xffffffff ^ node_index);
    // 删除 节点序号---节点类 映射
    index2nodeptr.erase(node_index);
    guard1.unlock();
    guard2.unlock();
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
void AccpetThread(Broke* const b) {
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t len;
    len = sizeof(cliaddr);
    while (1) {
        printf("Wait a new client...\n");
        clifd = b->WaitAccpet(cliaddr);
        if (clifd == -1)
            perror("accpet error:");
        else
        {
            printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
            // 新的客户端连接
            thread t(ConnectThread, b, clifd, cliaddr);
            t.detach();
        }
    }
}
void ConnectThread(Broke* const b, int connectfd, struct sockaddr_in cliaddr) {
    Node* mynode = new Node(inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port, connectfd);
    int32_t index_temp = b->AllocIndex();
    if (index_temp <= 0 || mynode->SetIndex(index_temp) <= 0) {
        // 序号分配/设置出错
        mynode->SetState(_connect_wait);
    }
    else {
        mynode->SetState(_connecting);
    }
    switch (mynode->GetState())
    {
        case _connect_wait:
        {
            // 此线程一直尝试 获取序号/分配序号
            thread t([b, mynode](){
                int32_t index_temp = 0x00000000;
                while (index_temp <= 0 || mynode->SetIndex(index_temp) == 0 )
                    index_temp = b->AllocIndex();
            });
            t.detach();
            // 调用重设置函数，一直阻塞在这里，默认等待时间 3 秒
            if (mynode->ResetIndex()) {
                mynode->SetState(_connecting);
            }
            else {
                mynode->SetState(_close);
                break;
            }
        }
        case _connecting:
        {
            // 添加 mynode 进入 broke 的管理
            // 加入 broke 成功
            if (b->AddNode(mynode)) {
                // 设置状态
                mynode->SetState(_connected);
                // 开启 读写 线程
                thread t1(ReadThread, b, mynode);
                thread t2(WriteThread, b, mynode);
                t1.detach();
                t2.detach();
            }
            else {
            // 加入 broke 失败
                // 设置状态
                mynode->SetState(_close);
            }
            break;
        }
        default:
        {
            printf("something error happened in [ConnectThread, switch]\n");
            delete mynode;
            break;
        }
    }
    mynode->WaitForClose();
    // broke 删除 mynode
    b->DelNode(mynode);
    delete mynode;
}
void ReadThread(Broke* const b, Node* const node) {
    
}
void WriteThread(Broke* const b, Node* const node) {
    
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
int main() {
    Broke* mybroke = new Broke;
    mybroke->StartServer(IPADDRESS, PORT);
}