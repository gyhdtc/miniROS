#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>

using namespace std;

int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM,0);
    if(socket_fd == -1)
    {
        cout<<"socket 创建失败："<<endl;
        exit(-1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int res = connect(socket_fd,(struct sockaddr*)&addr,sizeof(addr));
    if(res == -1)
    {
        cout<<"bind 链接失败："<<endl;
        exit(-1);
    }
    cout<<"bind 链接成功："<<endl;

    write(socket_fd,"hello hebinbing",15);
    const char s[4] = "gyh";
    //cin >> s;
    write(socket_fd,s,3);
    char buffer[255]={};
    int size = read(socket_fd, buffer, sizeof(buffer));//通过fd与客户端联系在一起,返回接收到的字节数
    //第一个参数：accept 返回的文件描述符
    //第二个参数：存放读取的内容
    //第三个参数：内容的大小

    cout << "接收到字节数为： " << size << endl;
    cout << "内容： " << buffer << endl;

    int t;
    cin >> t;
    while(t != 0)
    {
        cin >> t;
    }

    close(socket_fd);

    return 0;
}