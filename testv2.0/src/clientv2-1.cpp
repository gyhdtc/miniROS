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
            string ans = "";
            ans += Convert(x);
            ans += Convert(y);
            ans += Convert(z);
            return ans;
        }
        void String2Class(string s) {
            memcpy(&x, s.c_str(), 4);
            memcpy(&y, s.c_str()+4, 4);
            memcpy(&z, s.c_str()+8, 4);
        }
        void String2Class(const char *c, size_t len) {
            if (len == 12) {
                memcpy(&x, c, 4);
                memcpy(&y, c+4, 4);
                memcpy(&z, c+8, 4);
            }
            else {
                printf("len is not 12\n");
            }
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
    mynode->AddPub("zuobiao");
    float x = 1.2;
    float y = 3.4;
    float z = 5.6;
    ZuoBiao zb(x, y, z);
    for (int i = 0; i < 20; i++)
        mynode->SendData("zuobiao", zb.Set(x+i, y, z).Class2String());

    signal(SIGINT, SigThread);
    while (KeepRunning);
    delete mynode;
    return 0;
}