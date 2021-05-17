#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <memory.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>

#define IPADDRESS   "127.0.0.1"
#define PORT        8787
#define MAXSIZE     1024
#define LISTENQ     5
#define FDSIZE      1000
#define EPOLLEVENTS 100

//函数声明
//创建套接字并进行绑定
int socket_bind(const char* ip,int port);
//IO多路复用epoll
void do_epoll(int listenfd);
//事件处理函数
void handle_events(int epollfd,struct epoll_event *events,int num,int listenfd,char *buf);
//处理接收到的连接
void handle_accpet(int epollfd,int listenfd);
//读处理
void do_read(int epollfd,int fd,char *buf);
//写处理
void do_write(int epollfd,int fd,char *buf);
//添加事件
void add_event(int epollfd,int fd,int state);
//修改事件
void modify_event(int epollfd,int fd,int state);
//删除事件
void delete_event(int epollfd,int fd,int state);

int main() {
    int listenfd;
    listenfd = socket_bind(IPADDRESS, PORT);
    listen(listenfd, LISTENQ);
    do_epoll(listenfd);
    return 0;
}
int socket_bind(const char* ip,int port) {
    int listenfd;
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket error\n");
        exit(1);
    }
    // 清空结构体
    memset(&serveraddr, 0, sizeof(serveraddr));
    // 给结构体赋值，转换格式
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serveraddr.sin_addr);
    serveraddr.sin_port = htonl(port);
    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind error\n");
        exit(1);
    }
    return listenfd;
}
void do_epoll(int listenfd) {
    int epollfd;
    // 从内核返回的，得到的文件描述符集合
    struct epoll_event events[EPOLLEVENTS];
    int ret;
    char buffer[MAXSIZE];
    memset(buffer, 0, MAXSIZE);
    epollfd = epoll_create(FDSIZE);
    add_event(epollfd, listenfd, EPOLLIN);
    while (1) {
        ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
        handle_events(epollfd, events, ret, listenfd, buffer);
    }
    close(epollfd);
}
void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, char *buf) {
    int fd;
    for (int i = 0; i < num; i++) {
        fd = events[i].data.fd;
        if ( (fd == listenfd) && (events[i].events & EPOLLIN) ) {
            // listenfd 有新连接
            handle_accpet(epollfd, listenfd);
        }
        else if (events[i].events & EPOLLIN) {
            // fd 不是 listenfd，所以是 连接描述符
            do_read(epollfd, fd, buf);
        }
        else if (events[i].events & EPOLLOUT) {
            // fd 不是 listenfd，所以是 连接描述符
            do_write(epollfd, fd, buf);
        }
    }
}
void handle_accpet(int epollfd, int listenfd) {
    int newconnectfd;
    struct sockaddr_in newconnectaddr;
    socklen_t newconnectlen;
    newconnectfd = accept(listenfd, (struct sockaddr*)&newconnectaddr, &newconnectlen);
    if (newconnectfd == -1) {
        perror("accpet error\n");
    }
    else {
        printf("accept a new client: %s:%d\n", inet_ntoa(newconnectaddr.sin_addr), newconnectaddr.sin_port);
        add_event(epollfd, listenfd, EPOLLIN);
    }
}
void do_read(int epollfd, int fd, char *buf) {
    int nread;
    nread = read(fd, buf, MAXSIZE);
    if (nread == -1) {
        perror("read error\n");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    }
    else if (nread == 0) {
        fprintf(stderr, "client close\n");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    }
    else {
        printf("read message is : %s\n", buf);
        // 修改当前连接为写，因为是回射服务器嘛
        modify_event(epollfd, fd, EPOLLOUT);
    }
}
void do_write(int epollfd, int fd, char *buf) {
    int nwrite;
    nwrite = write(fd, buf, strlen(buf));
    if (nwrite == -1) {
        perror("write error\n");
        close(fd);
        delete_event(epollfd, fd, EPOLLOUT);
    }
    else {
        modify_event(epollfd, fd, EPOLLOUT);
    }
    memset(buf, 0, MAXSIZE);
}
void add_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}
void delete_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}
void modify_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}