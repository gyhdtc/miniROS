// 为了与 ROS 交互，需要调用 ROS C++ APIs
#include <ros/ros.h>
#include <node.h>

/* -------------------------------------- */
void MyServerCallBack(int *fd, struct sockaddr_in *client, RosNode *m) {
    /* rewrite */
    char *ip = inet_ntoa(client->sin_addr);
    char endflag[2] = {'#', '*'};
    char buffer[255]={};
    int dataflag = 0;
    int size = 0;
    while(dataflag == 0)
    {
        size = read(*fd, buffer, sizeof(buffer));
        if (size == 0 || size == -1) 
        {
            write(*fd, endflag+1, 1);
            continue;
        } else {
            cout << buffer << endl;
            write(*fd, endflag, 1);
            dataflag = 1;
        }
    }
    /* rewrite */
    close(*fd);
}

void MyClientCallBack(int *socket_fd, string s) {
    /* rewrite */
    cout << "6" << endl;
    char flag[1] = {'\0'};
    int x = 0;
    while (*flag != '#')
    {
        cout << "7" << endl;
        char *t = new char(s.length()+1);
        strcpy(t, s.c_str());
        write(*socket_fd, t, s.length());
        cout << "8" << endl;
        read(*socket_fd, flag, 1);
        cout << "9" << endl;
        delete []t;
    }
    /* rewrite */
}
/* -------------------------------------- */

void startros(int argc, char** argv) {
    // 该命令告诉 ROS 初始化了一个 node，名为 hello_world_node 
    ros::init(argc, argv, "hello_world_node");
    
    // 在一般的 ROS node 程序中，我们会用 ros::NodeHandle nh 来启动 node 程序，
    // ros::NodeHandle nh 默认会调用 ros::start() 函数，程序关闭时也会自动调用 ros::shutdown() 函数。
    // 我们也可以直接通过 ros::start() 和 ros::shutdown() 来手动控制 node 的开启和关闭
    ros::start();
    
    // 显示 hello, world! 信息
    ROS_INFO_STREAM("Hello, world!");
        // 用 ros::spin() 保持该程序运行，一直等待处理 subscribe 的数据
    // 由于该程序并没有 sub，所以就是简单的保持程序不退出而已， 直到接受到终止信号 SIGINT (ctrl-c)
    ros::spin();

    // 关闭 node 程序
    ros::shutdown();
}

int main(int argc, char** argv) {
    
    thread t(startros, argc, argv);
    t.detach();
    /* -------------------------------------- */
    string name = "gyh1";
    int port = 8889;
    // char *ip = (char *)"49.123.118.159";
    char *ip = (char *)"0.0.0.0";
    RosNode node1(port, ip, MyServerCallBack, MyClientCallBack);
    StartServer(&node1);
    
    int master_port = 8888;
    // char *master_ip = (char *)"115.157.195.140";
    char *master_ip = (char *)"127.0.0.1";

    node1.Reg(port, ip, master_port, master_ip, name);
    //node1.Pub("blue1");
    // for (int i = 0; i < 1; i++) 
    // {
    //     vector<int> a;
    //     for (int j = 0; j < 25; j++)
    //         a.push_back(i*20+j);
    //     node1.Data("blue1", a);
    // }
    signal(SIGINT, SigThread);

    while (keepRunning);
    /* -------------------------------------- */
    
    // 结束主程序
    return 0;
}
/*
g++ hello_world_node.cpp -o ./bin/hello_world_node -I/opt/ros/kinetic/include -I/home/gyh/shit/cpp/include -L/opt/ros/kinetic/lib -Wl,-rpath,/opt/ros/kinetic/lib -lroscpp -lrosconsole --std=c++11 -lpthread
*/