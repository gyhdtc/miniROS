# miniROS
1. cpp：存放c++文件；bin下存放执行文件；include存放头文件。
2. py：存放python文件。
3. test / test-ros：存放连接miniROS和ROS的调试文件。

## [C++] master
**Master开启两个线程：一个向外提供Server服务；一个用于遍历消息队列，queue非空即转发**  

- master节点是用c++写的，在cpp文件夹下，编译命令：  
    >g++ master.cpp -o ./bin/master --std=c++11 -lpthread  

- master.cpp文件包括两个回调函数，分别是：  
**(1)** `MyServerCallBack()`，当node连接master时，master作为服务器回调，处理node发来的数据。  
**(2)** `MyClientCallBack()`，当master连接node时，master作为客户端回调，对node转发队列数据。  

- `main()` 函数：main函数首先实例化一个master，需要提供master自己的ip和port。进一步启动一个线程，开启master服务器，`StartServer()`。在循环中不断判断master实例中的消息队列向量MQ是否为空，如果不为空就启动一个线程进行数据转发。  

- `master.cpp`包含`master.h`头文件。头文件实现类Master，此类继承于Server类。实例化Master时，也会实例化一个Server，所以创建Master时要提供master的IP和PORT。  

- `master.h`包含`client.h`，在需要数据转发时创建client类，提供目标主机的IP和PORT。还需要一个string类型的test文本参数，作为此client发送出去的数据。  

- `master.h`包含`header.h`。此头文件包含所有标准库文件。  
**(1)** 定义了类，常量，信号变量`keeprunning`等。同时定了三个函数指针类型： `ServerCallBack`，`NodeCallBack`，`ClientCallBack`。  
**(2)** 定义了**node**结构体，包含**node**创建时的自身信息，同时它也将保存在 master 节点中。定义了**MessageQueue**结构体，表示消息队列，包含消息名字，哪个节点发布的，哪些节点订阅了和**数据queue**。  
回调函数指针类型名：  
    > typedef decltype(stest)* ServerCallBack;  
    typedef decltype(ntest)* NodeCallBack;  
    typedef decltype(ctest)* ClientCallBack; 

    **(3)** 自己写了`mystrncpy`函数，调用`strncpy`函数之后在`char *`结尾补上了结束符。（标准库函数不会自动补）  

## [C++]node
**node开启一个向外提供Server服务的线程, 接收Master转发的数据**
- node节点可以用c++、python编写。在cpp文件夹下，编译命令：  
    >g++ node.cpp -o ./bin/node --std=c++11 -lpthread  

- node也需要拥有server和client回调函数；回调函数指针的定义跟Master一样。  

- `node.cpp`文件包含`node.h`头文件。头文件中实现了Node类，同样继承于Server类。在`node.cpp`文件中实例化Node类时，要提供本机IP和PORT，以供创建Server服务线程；同时要提供Master的IP和PORT，以供Node连接Master。Master的IP和PORT要事先配置好。（将来Master部署在实验室的服务器上就不用了）  

- Reg、Sub、Pub、Data函数，分别提供“节点注册、消息订阅（subscribe）、消息发布（publish），数据发布”功能。节点通过创建线程，向Master发送特定格式的数据实现。  
    > Reg：{REG}[name:{node.name};ip:{node.ip};port:{node.port};]  
    Sub：{SUB}[name:{node.name};sub:{sub_name};]  
    Pub：{PUB}[name:{node.name};pub:{pub_name};]  
    Data：{DATA}[name:{node.name};pub:{pub_name;{{data,}......}]  
    
    大括号{ }内部表示可变参数，实际传输数据没有{ }  

---
## [补充]使用说明  
node和master中的回调函数可以自己重写。回调函数指针定义为：
```cpp
void CallBackfun(void *param);  
typedef decltype(CallBackfun)* ServerCallBack;  
typedef decltype(CallBackfun)* NodeCallBack;  
typedef decltype(CallBackfun)* ClientCallBack;  
```
函数参数为 `void *`类型，意味着可以自定义任何形式的参数。只需要利用 `header.h` 中定义的参数结构体。例如默认的，在`header.h`中定义了三种回调函数的参数：
```cpp
/* header.h : serverparam */
struct ServerParam {
    int *fd;
    struct sockaddr_in *client;
    Master *m;
};
/* header.h : clientparam */
struct ClientParam {
    int socket_fd;
    string s;
};
/* header.h : nodeparam */
struct NodeParam {
    int *fd;
    struct sockaddr_in *client;
    RosNode *n;
};
```
结构体含义，顾名思义。在回调函数中，使用 param 指针时，记得进行类型转换：
```cpp
ServerParam *sp = (ServerParam *)param;
int *fd = sp->fd;
struct sockaddr_in *client = sp->client;
Master *m = sp->m;
```

## [补充]需求说明  
需要参考MQTT传输协议，将传输的内容包装成“二进制流”的形式。socket的开头几个字节设计成“数据包”的形式。  
