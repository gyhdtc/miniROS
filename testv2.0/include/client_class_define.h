#ifndef CLIENT_CLASS
#define CLIENT_CLASS
#include "./client_class.h"
#include "./client_thread.h"
/* ---------------------------------------------------------------------- */
int Client::ConnectBroke() {    
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        printf("Error: socket\n");
        return 0;
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(serverPort);
    dest_addr.sin_addr.s_addr = inet_addr(serverIp.c_str());
    if (connect(socketFd, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
        printf("Error : connect\n");
        return 0;
    }
    return socketFd;
}
/* ---------------------------------------------------------------------- */
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
void Node::WaitForClose() {
    unique_lock<mutex> lk(StateLock);
    CloseCv.wait(lk, [this](){ return State == _close; });
}
bool Node::AddSub(string name) {
    if (subTopicList.find(name) == subTopicList.end()) {
        subTopicList.insert(name);
        return true;
    }
    printf("already have this topic\n");
    return false;
}
bool Node::AddPub(string name) {
    if (pubTopicList.find(name) == pubTopicList.end()) {
        pubTopicList.insert(name);
        return true;
    }
    printf("already have this topic\n");
    return false;
}
bool Node::deltopic(string name) {
    if (pubTopicList.find(name) != pubTopicList.end()) {
        pubTopicList.erase(name);
        return true;
    }
    else if (subTopicList.find(name) != subTopicList.end()) {
        subTopicList.erase(name);
        return true;
    }
    else {
        printf("No this topic\n");
        return false;
    }
}
void Node::CloseWrite() {
    sem_post(&Msg_Sem);
}
void Node::SendReg(Msg &message) {
    unique_lock<mutex> lk(MsgLock);
    printf("send reg\n");
    Message.push_back(message);
    sem_post(&Msg_Sem);
}
void Node::SendData(Msg &message) {
    unique_lock<mutex> lk(MsgLock);
    printf("send data\n");
    Message.push_back(message);
    sem_post(&Msg_Sem);
}
void Node::SendTopic(Msg &message) {
    unique_lock<mutex> lk(MsgLock);
    printf("send topic\n");
    Message.push_back(message);
    sem_post(&Msg_Sem);
}
void Node::WaitForSendMsg() {
    sem_wait(&Msg_Sem);
}
Msg Node::GetTopMsg() {
    unique_lock<mutex> lk(MsgLock);
    Msg msg = Message.front();
    Message.pop_front();
    return msg;
}
void Node::GetData(Msg msg) {
    unique_lock<mutex> lk(MsgLock);
    RecvMessage.push_back(msg);
    RecvCv.notify_one();
}
Msg Node::WaitForData() {
    unique_lock<mutex> lk(MsgLock);
    RecvCv.wait(lk);
    Msg m = RecvMessage.front();
    RecvMessage.pop_front();
    return m;
}
/* ---------------------------------------------------------------------- */
void MyNode::StartClient(string name) {
    int fd;
    if (fd = client->ConnectBroke()) {
        printf("Success start client\n");
        node = make_shared<Node>(name, fd);
        thread t(ManageClient, this, node);
        t.detach();
    }
    else {
        printf("fail start client\n");
    }
}
void MyNode::Reg() {
    Msg msg;
    string name = node->GetName();
    shared_ptr<char> buffer(new char[8+name.size()]);
    msg.buffer = buffer;
    msg.head.data_len = name.size();
    msg.head.type = regnode;
    msg.head.check_code = codeGenera(name.c_str(), name.size());
    string2Msg(msg, "", name);
    node->SendReg(msg);
    int i = 0;
    while (i != 10 && node->GetState() != _online) {
        sleep(1);
        i++;
    }
}
void MyNode::AddSub(string topicname) {
    if (node->AddSub(topicname)) {
        Msg msg;
        uint32_t index = node->GetIndex();
        int len = topicname.size();
        shared_ptr<char> buffer(new char[8+len]);
        msg.buffer = buffer;
        msg.head.type = subtopic;
        msg.head.node_index = node_index2int8(index);
        msg.head.topic_name_len = len;
        msg.head.check_code = codeGenera(topicname.c_str(), len);
        string2Msg(msg, topicname, "");
        node->SendTopic(msg);
    }
}
void MyNode::AddPub(string topicname) {
    if (node->AddPub(topicname)) {
        Msg msg;
        uint32_t index = node->GetIndex();
        int len = topicname.size();
        shared_ptr<char> buffer(new char[8+len]);
        msg.buffer = buffer;
        msg.head.type = pubtopic;
        msg.head.node_index = node_index2int8(index);
        msg.head.topic_name_len = len;
        msg.head.check_code = codeGenera(topicname.c_str(), len);
        string2Msg(msg, topicname, "");
        node->SendTopic(msg);
    }
}
void MyNode::DelTopic(string topicname) {
    if (node->deltopic(topicname)) {
        Msg msg;
        uint32_t index = node->GetIndex();
        int len = topicname.size();
        shared_ptr<char> buffer(new char[8+len]);
        msg.buffer = buffer;
        msg.head.type = deltopic;
        msg.head.node_index = node_index2int8(index);
        msg.head.topic_name_len = len;
        msg.head.check_code = codeGenera(topicname.c_str(), len);
        string2Msg(msg, topicname, "");
        node->SendTopic(msg);
    }
}
void MyNode::SendData(string topicname, string tdata) {
    Msg msg;
    uint32_t index = node->GetIndex();
    int len = topicname.size() + tdata.size();
    shared_ptr<char> buffer(new char[8+len]);
    msg.buffer = buffer;
    msg.head.type = data;
    msg.head.node_index = node_index2int8(index);
    msg.head.topic_name_len = topicname.size();
    msg.head.data_len = tdata.size();
    msg.head.check_code = codeGenera(string(topicname+tdata).c_str(), len);
    string2Msg(msg, topicname, tdata);
    node->SendData(msg);
}
Msg MyNode::WaitForData() {
    return node->WaitForData();
}
void MyNode::MsgHandler(shared_ptr<Node> node, shared_ptr<char> buffer, Head head) {
    Msg msg;
    switch (head.type)
    {
        case getheartbeat:
        {
            printf("get node index\n");
            
            break;
        }
        case getregnode:
        {
            printf("get node index, reg success ： %d\n", head.return_node_index);
            node->SetIndex(int82node_index(head.return_node_index));
            node->SetState(_online);
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
            msg.buffer = buffer;
            msg.head = head;
            node->GetData(msg);
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