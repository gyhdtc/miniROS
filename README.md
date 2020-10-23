# miniROS
--cpp：存放c++文件；bin下存放执行文件；include存放头文件；  
--py：存放python文件
## [C++] master
- master节点是用c++写的，在cpp文件夹下，编译命令：  
**g++ master.cpp -o ./bin/master --std=c++11 -lpthread**  
- master.cpp文件包括两个回调函数两个回调函数分别是：
MyServerCallBack()，当node连接master时，master作为服务器回调，处理node发来的数据。  
MyClientCallBack()，当master连接node时，master作为客户端回调，对node转发队列数据。  
还有一个main() 函数。main函数首先实例化一个master，需要提供master自己的ip和port。进一步启动一个线程，开启master服务器，StartServer()。在循环中不断判断master实例中的消息队列向量MQ是否为空，如果不为空就启动一个线程进行数据转发。
- master直接包含master.h头文件。头文件实现类Master，此类继承于Server类。

## node
