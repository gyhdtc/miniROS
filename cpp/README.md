# miniROS
1. cpp：存放c++文件；bin下存放执行文件；include存放头文件。
2. py：存放python文件。
3. test / test-ros：存放连接miniROS和ROS的调试文件。  

### 记录一些坑
1. 出现“segmentation fault core dumped”错误时的调试方法：添加打印语句，二分法打印；利用**starce**看系统调用；使用gdb命令来调试。  
2. 你定义了一个结构体指针p，用来指向此类结构体，但是你却没有给他赋值，此时p的值为NULL，你并没有在内存中为p分配任何空间，所以p->a=1这句就会出段错误。  
3. new在分配内存时会调用默认的构造函数，而malloc不会调用。关于malloc()与new()的区别要重点复习复习。用C++开发程序时，就尽量使用C++中的函数，不要C++与C混合编程。  
4. string 的坑：https://www.cnblogs.com/lanxuezaipiao/p/3704578.html
5. 【资料】https://blog.csdn.net/s2152637/article/details/98987333
