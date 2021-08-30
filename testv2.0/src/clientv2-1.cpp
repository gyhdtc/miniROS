#include "../include/client_head.h"
#include "../include/client_thread.h"
#include "../include/client_class.h"
#include "../include/client_class_define.h"
/* -------------------要在这个定义自己的数据类型----------- */
/* 
请一定要继承UserDataBase类
并且实现纯虚函数方法
class UserDataBase {
    public:
        // 类数据转string
        virtual string Class2String() const = 0;
        // string转类数据
        virtual void String2Class(string) = 0;
        // char指针转类数据
        virtual void String2Class(const char *, size_t len) = 0;
};
*/
string Convert(float Num)
{
    std::ostringstream oss;
    oss<<Num;
    std::string str(oss.str());
    return str;
}
class ZuoBiao : public UserDataBase{
    private:
        float x;
        float y;
        float z;
    public:
        ZuoBiao& Set(float a, float b, float c) {
            x = a; y = b; z = c;
            return *this;
        }
        string Class2String() const {
            std::string res;
            char split = ';';
            res.append(std::to_string(x));
            res.push_back(split);
            res.append(std::to_string(y));
            res.push_back(split);
            res.append(std::to_string(z));
            res.push_back(split);
            return res;
        }
        void String2Class(string str) {
            float parse[3] = {0};
            int idx = 0, next = -1;
            char split = ';';    
            try {
                for(int i = 0; i < 1; ++i) {
                    idx = next + 1;
                    next = str.find(split, idx);
                    parse[i] = std::stod(str.substr(idx, next - idx));
                }
            } catch (...) {
                std::cout << " " << std::endl;
                this-> x = 0.0;
                this-> y = 0.0;
                this-> z = 0.0;
            }
            this-> x = parse[0];
            this-> y = parse[1];
            this-> z = parse[2];
        }
        void String2Class(const char *, size_t len) {

        }
        void print() const {
            printf("%f\n%f\n%f\n", x, y, z);
        }
        ZuoBiao(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
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
    string myname;
    cout << "input node name:\n";
    cin >> myname;
    // 创建一个 mynode 类
    MyNode* mynode = new MyNode(serverip, serverport);
    mynode->StartClient(myname);
    // 注册
    mynode->Reg();
    // 发布
    mynode->AddPub("movetcp");
    
    ZuoBiao a(0, 0, 0);
    int flag = 1;
    while (flag) {
        float x, y, z;
        cin >> x >> y >> z >> flag;
        mynode->SendData("movetcp", a.Set(x, y, z).Class2String());
    }

    signal(SIGINT, SigThread);
    while (KeepRunning);
    delete mynode;
    return 0;
}