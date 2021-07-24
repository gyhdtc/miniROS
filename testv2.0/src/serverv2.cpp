#include "../include/headv2.0.h"
/* 声明一些线程函数 */
// 等待连接线程
void AccpetThread(Broke* const);
// 处理/管理 新连接
void ConnectThread(Broke* const, int, sockaddr_in);
// 新节点的 读/写 线程
void ReadThread(Broke* const, shared_ptr<Node> mynode);
void WriteThread(Broke* const, shared_ptr<Node> mynode);
// broke进行数据转发
void MsgCopyToNode(Msg);
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
    void printName(string);
    void printIndex(uint32_t);
    void printAll();
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
void Topic::printAll() {
    for (auto i : name2subANDpub) {
        printName(i.first);
    }
}
void Topic::printName(string name) {
	cout << "----------------话题信息-------------------\n";
	if (name2subANDpub.find(name) == name2subANDpub.end()) {
		cout << "没有" << name << "话题的相关信息\n";
	}
	else {
		// 输出为二进制
		auto idTobit = [](uint32_t index)->string {
			string res = "0000 0000 0000 0000 0000 0000 0000 0000";
			int i = 0;
			while (index) {
				if (index & 1) {
					res[38 - (i + i / 4)] = '1';
				}
				i++;
				index = index >> 1;
			}
			return res;
		};
		// 返回1的个数
		auto numOf1 = [](uint32_t num)->int {
			int res = 0;
			while (num) {
				res++;
				num = num & (num - 1);
			}
			return res;
		};
		cout << name << "话题的发布者：\n";
		if (numOf1(name2subANDpub[name].first) != 1) {
			cout << "此话题没有发布者！\n";
		}
		else {
			cout << idTobit(name2subANDpub[name].first) << "\n";
		}
		cout << name << "话题的订阅者：\n";
		if (name2subANDpub[name].first == 0) {
			cout << "此话题还没有订阅者！\n";
		}
		else {
			cout << idTobit(name2subANDpub[name].second) << "\n";
		}
	}
	cout << "-----------------------------------------\n";
}
void Topic::printIndex(uint32_t index) {
	// 输出为二进制
	auto idTobit = [](uint32_t index)->string {
		string res = "0000 0000 0000 0000 0000 0000 0000 0000";
		int i = 0;
		while (index) {
			if (index & 1) {
				res[38 - (i + i / 4)] = '1';
			}
			i++;
			index = index >> 1;
		}
		return res;
	};
	string myid = idTobit(index);
	cout << "----------------节点信息-------------------\n";
	if (sub2name.find(index) == sub2name.end()) {
		cout << "节点" << myid << "没有发布话题！\n";
	}
	else {
		cout << "节点" << myid << "发布的话题：\n";
		for (auto name : sub2name[index]) {
			cout << name << ", ";
		}
		cout << "\n";
	}
	if (pub2name.find(index) == pub2name.end()) {
		cout << "节点" << myid << "没有订阅话题！\n";
	}
	else {
		cout << "节点" << myid << "订阅的话题：\n";
		for (auto name : pub2name[index]) {
			cout << name << ", ";
		}
		cout << "\n";
	}
	cout << "-----------------------------------------\n";
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
class Node
{
private:
    // 状态
    int State;
    mutex StateLock;
    // 关闭状态的条件变量
    condition_variable CloseCv;
    // 节点序号
    int32_t nodeIndex;
    bool IndexFlag;
    bool TimeOutflag;
    mutex IndexLock;
    // 序号重获的条件变量
    condition_variable IndexResetCv;
    // 连接后，不变
    string nodeIp;
    int nodePort;
    int connectfd;
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
    // 在创建多线程的时候，用来保护node数据不出错
    // 有时候也用来保护 shared ptr 的引用记数正确
    mutex ProtectThread;
    int GetConnFd() const { return connectfd; }
    // 节点序列号设置
    int32_t GetIndex();
    bool ResetIndex(int);
    int SetIndex(int32_t);
    // 节点状态设置
    int GetState();
    void SetState(int);
    // 等待节点关闭
    void WaitForClose();
    // 发送函数
    void SendMsg();
    void CloseWrite();
    void SendCheckHeart(Msg);
    void SendCheckReg(Msg);
    void SendData(Msg);
    void SetName(string name) { nodeName = name; }
    Msg GetTopMsg();
    // 处理话题信息
    void AddSub(string name) { subTopicList.insert(name); }
    void AddPub(string name) { pubTopicList.insert(name); }
    void deltopic(string);
    Node(string ip, int port, int fd)
    {
        nodeIp = ip;
        nodePort = port;
        connectfd = fd;
        nodeIndex = 0x00000000;
        State = _newconnect;
        IndexFlag = false;
        TimeOutflag = false;
        // 初始化信号量
        int ret = sem_init(&Msg_Sem, 0, 0);
        printf("Create a node = %s:%d\n", nodeIp.c_str(), nodePort);
    }
    ~Node()
    {
        printf("Close a Node = %s:%d %s\n", nodeIp.c_str(), nodePort, nodeName.c_str());
        close(connectfd);
        sem_destroy(&Msg_Sem);
    }
};
// -1: 超时 。 0 : 已经设置 。1 : 设置成功
int Node::SetIndex(int32_t index) {
    unique_lock<mutex> lk(IndexLock);
    printf("%d to node %s:%d\n", index, nodeIp.c_str(), nodePort);
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
    // 要么 超时，要么IndexFlag 设置好了之后被唤醒
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
void Node::SendData(Msg message) {
    unique_lock<mutex> lk(MsgLock);
    Message.push_back(message);
    sem_post(&Msg_Sem);
}
void Node::SendCheckHeart(Msg message) {
    unique_lock<mutex> lk(MsgLock);
    Message.push_back(message);
    sem_post(&Msg_Sem);
}
void Node::SendCheckReg(Msg message) {
    unique_lock<mutex> lk(MsgLock);
    Message.push_back(message);
    sem_post(&Msg_Sem);
}
void Node::CloseWrite() {
    sem_post(&Msg_Sem);
}
void Node::SendMsg() {
    sem_wait(&Msg_Sem);
}
void Node::deltopic(string name) {
    if (pubTopicList.find(name) != pubTopicList.end()) {
        pubTopicList.erase(name);
    }
    else if (subTopicList.find(name) != subTopicList.end()) {
        subTopicList.erase(name);
    }
    else {
        printf("No this topic\n");
    }
}
Msg Node::GetTopMsg() {
    unique_lock<mutex> lk(MsgLock);
    Msg msg = Message.front();
    Message.pop_front();
    return msg;
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
    map<int32_t, shared_ptr<Node>> index2nodeptr;
    mutex Index2NodeptrLock;
    // 数据有关的成员
    deque<Msg> Message;
    mutex MsgLock;
    condition_variable MsgCv;
public:
    // 创建连接
    void StartServer(string, int);
    int WaitAccpet(struct sockaddr_in&);
    // 管理节点
    int32_t AllocIndex();
    bool AddNode(shared_ptr<Node>);
    bool DelNode(shared_ptr<Node>);
    // 处理数据的函数
    void MsgHandler(shared_ptr<Node>, shared_ptr<char>, Head);
    Broke()
    {
        totalNode = 0;
        topic = new Topic;
        server = NULL;
        printf("Start broke\n");
    }
    ~Broke()
    {
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
    thread t(AccpetThread, this);
    t.detach();
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
bool Broke::AddNode(shared_ptr<Node> node) {
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
bool Broke::DelNode(shared_ptr<Node> node) {
    // 先删索引，再恢复 totalnode
    // index 不一定存在，先判断
    unique_lock<mutex> lk(IndexLock);
    unique_lock<mutex> lk2(Index2NodeptrLock);
    int32_t node_index = node->GetIndex();
    printf("del node-%d\n", node_index);
    // 删除节点下对应的topic
    topic->delTopic("all", node_index);
    // 恢复节点序号
    totalNode = totalNode & (0xffffffff ^ node_index);
    // 删除 节点序号---节点类 映射
    index2nodeptr.erase(node_index);
    return true;
}
void Broke::MsgHandler(shared_ptr<Node> mynode, shared_ptr<char> buffer, Head head) {
    Msg msg;
    switch (head.type)
    {
        case heartbeat:
        {
            printf("get heartbeat\n");
            // 直接调用节点的心跳返回函数
            *buffer.get() = getheartbeat;
            msg.head =  head;
            msg.buffer = buffer;
            msg.head.type = getheartbeat;
            string2Msg(msg, "", "");
            mynode->SendCheckHeart(msg);
            break;
        }
        case getheartbeat:
        {
            printf("get getheartbeat\n");
            // broke 按道理是不会有确认心跳的
            // 应该调用节点类的client部分的函数。还没开始写呢。
            break;
        }
        // 对于 topic 的操作其实应该开线程，否则客户端一多就会堵
        case subtopic:
        {
            printf("get subtopic\n");
            // 节点订阅
            // 调用broke的函数创建映射关系
            // 调用节点自己的函数。添加进入node类中的subTopicList
            string topicname(buffer.get()+8, int(head.topic_name_len));
            uint32_t node_index = int82node_index(head.node_index);
            mynode->AddSub(topicname);
            topic->subTopic(topicname, node_index);
            topic->printAll();
            break;
        }
        case pubtopic:
        {
            printf("get pubtopic\n");
            // 发布订阅
            // 调用broke的函数创建映射关系
            // 调用节点自己的函数。添加进入node类中的pubTopicList
            string topicname(buffer.get()+8, int(head.topic_name_len));
            uint32_t node_index = int82node_index(head.node_index);
            mynode->AddPub(topicname);
            topic->pubTopic(topicname, node_index);
            topic->printAll();
            break;
        }
        case deltopic:
        {
            printf("get deltopic\n");
            // 删除订阅
            // 调用broke的函数删除映射关系
            // 调用节点自己的函数。从node类中的pubTopicList和subTopicList删除
            string topicname(buffer.get()+8, int(head.topic_name_len));
            uint32_t node_index = int82node_index(head.node_index);
            mynode->deltopic(topicname);
            topic->delTopic(topicname, node_index);
            topic->printAll();
            break;
        }
        // 对于 topic 的操作其实应该开线程，否则客户端一多就会堵
        case regnode:
        {
            printf("get regnode\n");
            // 节点注册
            // 转换节点状态，确认节点name；并返回确认注册的状态，这个直接调用node的确认注册函数
            string node_name(buffer.get()+8, head.data_len);
            mynode->SetName(node_name);
            msg.head.type = getregnode;
            msg.head.return_node_index = node_index2int8(mynode->GetIndex());
            msg.buffer = buffer;
            string2Msg(msg, "", "");
            mynode->SendCheckReg(msg);
            break;
        }
        case getregnode:
        {
            printf("get getregnode\n");
            // broke 按道理是不会受到这个的
            break;
        }
        case reconnect:
        {
            printf("get reconnect\n");
            // 未完待续
            break;
        }
        case data:
        {
            printf("get data\n");
            // 收到了数据
            // MsgCopyToNode 线程会被唤醒，并进行操作
            msg.head = head;
            msg.buffer = buffer;
            msg.head.node_index = 0b00000000;
            *(msg.buffer.get()+1) = 0;
            thread t(MsgCopyToNode, msg);
            t.detach();
            break;
        }
        default:
        {
            printf("error message head\n");
            break;
        }
    }
}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
void AccpetThread(Broke* const b) {
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t len;
    len = sizeof(cliaddr);
    printf("Wait client...\n");
    while (1) {
        clifd = b->WaitAccpet(cliaddr);
        if (clifd == -1)
            perror("accpet error:");
        else
        {
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
            // 新的客户端连接
            thread t(ConnectThread, b, clifd, cliaddr);
            t.detach();
        }
    }
}
void ConnectThread(Broke* const b, int connectfd, struct sockaddr_in cliaddr) {
    shared_ptr<Node> mynode(new Node(inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port, connectfd));
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
                mynode->ProtectThread.lock();
                thread t1(ReadThread, b, mynode);
                t1.detach();
                mynode->ProtectThread.unlock();
                thread t2(WriteThread, b, mynode);
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
            mynode->SetState(_close);
            break;
        }
    }
    mynode->WaitForClose();
    // broke 删除 mynode
    b->DelNode(mynode);
}
void ReadThread(Broke* const b, shared_ptr<Node> mynode) {
    mynode->ProtectThread.lock();
    printf("node %d start readthread\n", mynode->GetIndex());
    mynode->ProtectThread.unlock();

    int connectFd = mynode->GetConnFd();
    bool MsgFlag = true;
    mynode->SetState(_online);
    while (MsgFlag && mynode->GetState() != _close) {
        shared_ptr<char> buffer(new char[MAX_BUFFER_SIZE]);
        // 获取头部---------------------------------------------------------
        int RecvHeadLen = headlength;
        Head head;
        int MsgLen = 0;
        while (MsgFlag && RecvHeadLen > 0) {
            MsgLen = read(connectFd, buffer.get(), RecvHeadLen);
            if (MsgLen > 0)
                RecvHeadLen -= MsgLen;
            else
                MsgFlag = false;
        }
        if (MsgFlag) {
            GetHead(head, buffer.get());
        }
        else {
            continue;
        }
        // 获取数据---------------------------------------------------------
        int RecvTopicNameLen = head.topic_name_len;
        int RecvDataLen = head.data_len;
        int RecvBodylen = RecvTopicNameLen + RecvDataLen;
        while (MsgFlag && RecvBodylen > 0) {
            MsgLen = read(connectFd, buffer.get()+headlength, RecvBodylen);
            if (MsgLen > 0)
                RecvBodylen -= MsgLen;
            else
                MsgFlag = false;
        }
        out((uint8_t *)buffer.get(), headlength+head.topic_name_len+head.data_len);
        // 处理数据---------------------------------------------------------
        assert(head.check_code == codeGenera(buffer.get()+8, head.topic_name_len+head.data_len));
        if (MsgFlag) {
            b->MsgHandler(mynode, buffer, head);
        }
        else {
            continue;
        }
    }
    if (mynode->GetState() != _close) mynode->SetState(_close);
    mynode->CloseWrite();
    printf("read stop\n");
}
void WriteThread(Broke* const b, shared_ptr<Node> mynode) {
    mynode->ProtectThread.lock();
    printf("node %d start writethread\n", mynode->GetIndex());
    mynode->ProtectThread.unlock();
    int connectFd = mynode->GetConnFd();
    while (1) {
        // 等待被唤醒
        mynode->SendMsg();
        // 发送
        if (mynode->GetState() != _close) {
            Msg msg = mynode->GetTopMsg();
            write(connectFd, msg.buffer.get(), headlength + msg.head.topic_name_len + msg.head.data_len);
        }
        else {
            break;
        }
    }
    printf("write stop\n");
}
void MsgCopyToNode(Msg msg) {

}
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
int main() {
    Broke* mybroke = new Broke;
    mybroke->StartServer(IPADDRESS, PORT);
    signal(SIGINT, SigThread);
    while (KeepRunning);
    delete mybroke;
    return 0;
}