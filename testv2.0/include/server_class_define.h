#ifndef SERVER_CLASS
#define SERVER_CLASS
#include "./server_class.h"
#include "./server_thread.h"
/* ---------------------------------------------------------------------- */
void Server::Bind() {
    const char * ip = myIp.c_str();
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket error\n");
        exit(1);
    }
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
void Topic::pubTopic(string name, int32_t index) {
    unique_lock<mutex> lk(SubLock);
    unique_lock<mutex> lk2(NameLock);
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
        if (pub2name.find(index) == pub2name.end()) {
            pub2name.insert(pair<int32_t, set<string>>{index, set<string>{name}});
        }
        else {
            pub2name[index].insert(name);
        }
    }
    else {
        // 有这个话题，并且有话题发布者
        printf("%s already pubed by %d\n", name.c_str(), index);
    }
}
void Topic::subTopic(string name, int32_t index) {
    unique_lock<mutex> lk(PubLock);
    unique_lock<mutex> lk2(NameLock);
    if (name2subANDpub.find(name) == name2subANDpub.end() || (name2subANDpub.find(name) != name2subANDpub.end() && (name2subANDpub[name].second & index) == 0)) {
        name2subANDpub[name].second = name2subANDpub[name].second | index;
        sub2name[index].insert(name);
    }
    else {
        // 有这个话题，并且有话题订阅者
        printf("%s already subed by %d\n", name.c_str(), index);
    }
}
void Topic::delTopic(string name, int32_t index) {
    unique_lock<mutex> lk(SubLock);
    unique_lock<mutex> lk2(PubLock);
    unique_lock<mutex> lk3(NameLock);
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
    unique_lock<mutex> lk(NameLock);
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
		cout << name << " pub：\n";
		if (numOf1(name2subANDpub[name].first) != 1) {
			cout << "no pub\n";
		}
		else {
			cout << idTobit(name2subANDpub[name].first) << "\n";
		}
		cout << name << " sub：\n";
		if (name2subANDpub[name].second == 0) {
			cout << "no sub\n";
		}
		else {
			cout << idTobit(name2subANDpub[name].second) << "\n";
		}
	}
	cout << "-------------------------------------------\n";
}
void Topic::printIndex(uint32_t index) {
    unique_lock<mutex> lk(PubLock);
    unique_lock<mutex> lk2(SubLock);
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
uint32_t Topic::GetSubNodeSet(string topicname, uint32_t nodeindex) {
    unique_lock<mutex> lk(NameLock);
    if (name2subANDpub.find(topicname) != name2subANDpub.end() && ((name2subANDpub[topicname].first & nodeindex) != 0x00000000))
        return name2subANDpub[topicname].second;
    else
        return 0x00000000;
}
/* ---------------------------------------------------------------------- */
// -1: 超时 。 0 : 已经设置 。1 : 设置成功
int Node::SetIndex(int32_t index) {
    unique_lock<mutex> lk(IndexLock);
    printf("[%d] to node %s:%d\n", index, nodeIp.c_str(), nodePort);
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
        printf("[%d] Set Node State to %s\n", nodeIndex, DP[transferstate].c_str());
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
void Node::WaitForSendMsg() {
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
    printf("[%d] del node\n", node_index);
    // 删除节点下对应的topic
    topic->delTopic("all", node_index);
    // 恢复节点序号
    totalNode = totalNode & (0xffffffff ^ node_index);
    // 删除 节点序号---节点类 映射
    index2nodeptr.erase(node_index);
    return true;
}
uint32_t Broke::GetSubNodeSet(string name, uint32_t nodeindex) {
    return topic->GetSubNodeSet(name, nodeindex);
}
shared_ptr<Node> Broke::GetNodePtr(uint32_t index) {
    unique_lock<mutex> lk(Index2NodeptrLock);
    if (index2nodeptr.find(index) != index2nodeptr.end()) return index2nodeptr[index];
    return NULL;
}
void Broke::MsgHandler(shared_ptr<Node> mynode, shared_ptr<char> buffer, Head head) {
    Msg msg;
    switch (head.type)
    {
        case heartbeat:
        {
            printf("[%d] get heartbeat\n", mynode->GetIndex());
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
            printf("[%d] get subtopic\n", mynode->GetIndex());
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
            printf("[%d] get pubtopic\n", mynode->GetIndex());
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
            printf("[%d] get deltopic\n", mynode->GetIndex());
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
            printf("[%d] get regnode\n", mynode->GetIndex());
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
            printf("[%d] get data\n", mynode->GetIndex());
            // 收到了数据
            // MsgCopyToNode 线程会被唤醒，并进行操作
            msg.head = head;
            msg.buffer = buffer;
            msg.head.node_index = 0b00000000;
            *(msg.buffer.get()+1) = 0b00000000;
            uint32_t node_index = int82node_index(head.node_index);
            string topicname(buffer.get()+8, head.topic_name_len); 
            thread t(MsgCopyToNode, this, msg, topicname, node_index);
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
#endif