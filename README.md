# miniROS
1. cpp：存放c++文件；bin下存放执行文件；include存放头文件。
2. py：存放python文件。
3. test-ros：存放连接miniROS和ROS的调试文件。

## [C++] master
**Master开启两个线程，一个向外提供Server服务，一个用于遍历消息队列，非空即转发**
- master节点是用c++写的，在cpp文件夹下，编译命令：  
`g++ master.cpp -o ./bin/master --std=c++11 -lpthread`
- master.cpp文件包括两个回调函数，分别是：  
(1). MyServerCallBack()，当node连接master时，master作为服务器回调，处理node发来的数据。  
(2). MyClientCallBack()，当master连接node时，master作为客户端回调，对node转发队列数据。  
- main() 函数：main函数首先实例化一个master，需要提供master自己的ip和port。进一步启动一个线程，开启master服务器，StartServer()。在循环中不断判断master实例中的消息队列向量MQ是否为空，如果不为空就启动一个线程进行数据转发。
- master.cpp包含master.h头文件。头文件实现类Master，此类继承于Server类。实例化Master时，也会实例化一个Server，所以创建Master时要提供master的IP和PORT。
- `master.h`包含`client.h`，在需要数据转发时创建client类，提供目标主机的IP和PORT。还需要一个string类型的test文本参数，作为此client发送出去的数据。
- `master.h`包含`header.h`。此头文件包含所有标准库文件。  
定义了类，常量，信号变量`keeprunning`等。同时定了三个函数指针类型：ServerCallBack，NodeCallBack，ClientCallBack。  
定义了node结构体，包含node创建时的自身信息，同时它也将保存在master中。定义了MessageQueue结构体，表示消息队列，包含消息名字，哪个节点发布的，哪些节点订阅了和**数据queue**。  
自己写了mystrncpy函数，调用strncpy函数之后在char *结尾补上了结束符。（标准库函数不会自动补）

## node

