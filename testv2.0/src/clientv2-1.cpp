#include "../include/client_head.h"
#include "../include/client_thread.h"
#include "../include/client_class.h"
#include "../include/client_class_define.h"
/* -------------------按照这个定义自己的数据类型----------- */
/* 
请一定要继承UserDataBase类
并且实现纯虚函数方法
class UserDataBase {
    public:
        // 类数据转string
        virtual string Class2String() const = 0;
        // string转类数据
        virtual void String2Class(string) = 0;
};
*/
/* -------------------按照这个定义自己的数据类型----------- */
class ZuoBiao : public UserDataBase{
    private:
        float x;
        float y;
        float z;
        float rx;
        float ry;
        float rz;
    public:
        ZuoBiao& Set(float a, float b, float c, float d, float e, float f) {
            x = a; y = b; z = c; rx = d; ry = e; rz = f;
            return *this;
        }
        string Class2String() const {
            std::string res;
            char split = ';';
            res.append(std::to_string(x)); res.push_back(split);
            res.append(std::to_string(y)); res.push_back(split);
            res.append(std::to_string(z)); res.push_back(split);
            res.append(std::to_string(rx)); res.push_back(split);
            res.append(std::to_string(ry)); res.push_back(split);
            res.append(std::to_string(rz)); res.push_back(split);
            return res;
        }
        void String2Class(string str) {
            float parse[6] = {0};
            int idx = 0, next = -1;
            char split = ';';    
            try {
                for(int i = 0; i < 6; ++i) {
                    idx = next + 1;
                    next = str.find(split, idx);
                    parse[i] = std::stod(str.substr(idx, next - idx));
                }
            } catch (...) {
                std::cout << " " << std::endl;
                this-> x = 0.0;
                this-> y = 0.0;
                this-> z = 0.0;
                this-> rx = 0.0;
                this-> ry = 0.0;
                this-> rz = 0.0;
            }
            this-> x = parse[0];
            this-> y = parse[1];
            this-> z = parse[2];
            this-> rx = parse[3];
            this-> ry = parse[4];
            this-> rz = parse[5];
        }
        void print() const {
            printf("%f   %f   %f  %f   %f   %f", x, y, z, rx, ry,rz);
        }
};
class HandCommand : public UserDataBase{
    private:
        int pos;
        int force;
    public:
        HandCommand& Set(int a, int b) {
            pos = a; force = b;
            return *this;
        }
        string Class2String() const {
            std::string res;
            char split = ';';
            res.append(std::to_string(pos)); res.push_back(split);
            res.append(std::to_string(force)); res.push_back(split);
            return res;
        }
        void String2Class(string str) {
            float parse[2] = {0};
            int idx = 0, next = -1;
            char split = ';';    
            try {
                for(int i = 0; i < 2; ++i) {
                    idx = next + 1;
                    next = str.find(split, idx);
                    parse[i] = std::stod(str.substr(idx, next - idx));
                }
            } catch (...) {
                std::cout << " " << std::endl;
                this-> pos = 0;
                this-> force = 0;
            }
            this-> pos = parse[0];
            this-> force = parse[1];
        }
        void print() const {
            printf("%d   %d", pos, force);
        }
};
int main(int argc, char *argv[]) {
    // broke 代理的 ip 和 port
    string serverip = "127.0.0.1";
    int serverport = 8787;
    if (argc == 2) {
        serverip = argv[1];
    }
    if (argc == 3) {
        serverip = argv[1];
        serverport = stoi(string(argv[2]));
    }
    // 本节点的名字
    string myname = "gyh";
    // cout << "input node name:\n";
    // cin >> myname;
    // 创建一个 mynode 类
    MyNode* mynode = new MyNode(serverip, serverport);
    mynode->StartClient(myname);
    // 注册
    mynode->Reg();
    // 发布
    mynode->AddPub("movetcp");
    mynode->AddSub("movecheck");
    mynode->AddPub("hand");
    mynode->AddSub("handcheck");
    ZuoBiao zb;
    HandCommand hc;
    Msg t;
    vector<pair<string, string>> process;
    // process.push_back({"movetcp", zb.Set(0.489, 0.382, 0.181, 1.664, 0.2076, 0.1616).Class2String()});
    // process.push_back({"movetcp", zb.Set(0.38, 0.075, 0.55, 3.14165, 0.0, 0.0).Class2String()});
    process.push_back({"movetcp", zb.Set(0.489, 0.382, 0.181, 1.664, 0.2076, 0.1616).Class2String()});
    process.push_back({"hand", hc.Set(90, 10).Class2String()});
    process.push_back({"movetcp", zb.Set(0.489, 0.252, 0.181, 1.664, 0.2076, 0.1616).Class2String()});
    process.push_back({"hand", hc.Set(50, 30).Class2String()});
    process.push_back({"movetcp", zb.Set(0.489, 0.10, 0.431, 1.664, 0.2076, 0.1616).Class2String()});    
    process.push_back({"movetcp", zb.Set(0.576, -0.149, 0.440, 3.9060, 0.0581, 0.1289).Class2String()});
    printf("start process ------------------------->\n");
    for (auto gyh : process) {
        mynode->SendData(gyh.first, gyh.second);
        t = mynode->WaitForData();
        string topicname(t.buffer.get()+8, int(t.head.topic_name_len));
        string data(t.buffer.get()+8+int(t.head.topic_name_len), int(t.head.data_len));
        if (gyh.first == "movetcp") {
            if (topicname == "movecheck" && data == "ok;") {
                printf("move check\n");
                continue;
            }
            else {
                mynode->SendData(gyh.first, gyh.second);
            }
        }
        else if (gyh.first == "hand") {
            if (topicname == "handcheck" && data == "ok;") {
                printf("hand check\n");
                continue;
            }
            else {
                mynode->SendData(gyh.first, gyh.second);
            }
        }
        
    }
    printf("<------------------------- end process\n");
    // signal(SIGINT, SigThread);
    // while (KeepRunning);
    delete mynode;
    return 0;
}