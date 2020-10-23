# miniROS
--cpp：存放c++文件；bin下存放执行文件；include存放头文件；  
--py：存放python文件
## [C++] master
- master节点是用c++写的，在cpp文件夹下，编译命令：  
**g++ master.cpp -o ./bin/master --std=c++11 -lpthread**  
- master.cpp文件包括两个回调函数
- master直接包含master.h头文件。头文件实现类Master，此类继承于Server类。

## node