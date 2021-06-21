#include "headertest.h"
// 定义常量
#define IPADDRESS   "127.0.0.1"
#define PORT        8787
#define MAXSIZE     1024
#define LISTENQ     100

int main(){
    int fd;
    void *start;
    struct stat sb;
    fd = open("./mmap.txt", O_RDONLY); /*打开/etc/passwd */
    fstat(fd, &sb); /* 取得文件大小 */
    printf("mmap.txt file size = %d \n", (int)sb.st_size);
    // char * c = new char[1024];
    int mmapsize = (sb.st_size/4096+1)*4096;
    start = mmap(NULL, mmapsize, PROT_READ, MAP_PRIVATE, fd, 0);
    if(start == MAP_FAILED) /* 判断是否映射成功 */
        return 0;
    printf("%d\n", strlen((char*)start));
    printf("%s\n", start);
    /* 构建一个 client */
    int sockfd;
    struct sockaddr_in  servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    printf("connect OK\n");
    sleep(3);
    
    write(sockfd, start, 100);

    /* 构建一个 client */
    munmap(start, mmapsize); /* 解除映射 */
    close(fd);
    return 0;
}