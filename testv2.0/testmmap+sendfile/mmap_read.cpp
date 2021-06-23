#include "headertest.h"
// 定义常量
#define IPADDRESS   "127.0.0.1"
#define PORT        8787
#define MAXSIZE     1024
#define LISTENQ     100

int main(){
    // int fd;
    // void *start;
    // struct stat sb;
    // fd = open("./mmap.txt", O_RDONLY); /*打开/etc/passwd */
    // fstat(fd, &sb); /* 取得文件大小 */
    // printf("mmap.txt file size = %d \n", (int)sb.st_size);
    // // char * c = new char[1024];
    // int mmapsize = (sb.st_size/4096+1)*4096;
    // start = mmap(NULL, mmapsize, PROT_READ, MAP_PRIVATE, fd, 0);
    // if(start == MAP_FAILED) /* 判断是否映射成功 */
    //     return 0;
    // printf("%d\n", strlen((char*)start));
    // printf("%s\n", start);
    // munmap(start, mmapsize); /* 解除映射 */
    // close(fd);
    /* 构建一个 server */
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        std::cout << "Error: socket" << std::endl;
        return 0;
    }
    // bind
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8787);
    // addr.sin_addr.s_addr = IPADDRESS;
    inet_pton(AF_INET, IPADDRESS, &addr.sin_addr);
    if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cout << "Error: bind" << std::endl;
        return 0;
    }
    // listen
    if(listen(listenfd, 5) == -1) {
        std::cout << "Error: listen" << std::endl;
        return 0;
    }
    // accept
    int conn;
    char clientIP[INET_ADDRSTRLEN] = "";
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    while (true) {
        std::cout << "...listening" << std::endl;
        conn = accept(listenfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (conn < 0) {
            std::cout << "Error: accept" << std::endl;
            continue;
        }
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        std::cout << "...connect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

        char buf[255];
        
        char *start;
        struct stat sb;
        start = (char *)mmap(NULL, 100, PROT_READ, MAP_PRIVATE, conn, 0);
        // 这里是“连接描述符”
        // 按理说和文件描述符一个性质
        // 按理说应该可以被映射的
        std::cout << start << std::endl;

        while (true) {
            memset(buf, 0, sizeof(buf));
            int len = read(conn, start, sizeof(start));
            // buf[len] = '\0';
            // if (strcmp(buf, "exit") == 0) {
            //     std::cout << "...disconnect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
            //     break;
            // }
            // std::cout << start << std::endl;
            printf("%s\n", start);
            break;
            // send(conn, buf, len, 0);
        }
        
        close(conn);
    }
    close(listenfd);
    /* 构建一个 server */
    return 0;
}