#include "../include/headv2.0.h"

uint8_t string2uint8(string s) {
    uint8_t res = 0;
    for (auto c : s) {
        res = res * 10 + c - '0';
    }
    return res;
}
void readthread(int connectfd) {
    char *buffer = new char[1024];
    int MsgLen = 0;
    while (MsgLen = read(connectfd, buffer, 1) > 0) {
        cout << "get : " << bitset<8>(buffer[0]) << endl;
    }
    close(connectfd);
}
int main() {
    int sockClient = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(8787);
    addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = connect(sockClient, (struct sockaddr *)&addrSrv, sizeof(addrSrv));
    thread t(readthread, sockClient);
    t.detach();
    if (ret >= 0) {
        while (1) {
            uint8_t buffer[8];
            for (int i = 0; i < 8; i++) {
                cin >> buffer[i];
                buffer[i] -= '0';
                cout << bitset<8>(buffer[i]) << endl;
            }
            if (buffer[0] == 255) break;
            write(sockClient, buffer, 8);
        }
    }
    else {
        cout << "bye\n";
    }
    close(sockClient);
    return 0;
}