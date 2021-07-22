#include "../include/headv2.0.h"

uint8_t string2uint8(string s) {
    uint8_t res = 0;
    for (auto c : s) {
        res = res * 10 + c - '0';
    }
    return res;
}

int main() {
    int sockClient = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(8787);
    addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = connect(sockClient, (struct sockaddr *)&addrSrv, sizeof(addrSrv));
    if (ret >= 0) {
        cout << "send\n";
        char flag = 'a';
        while (flag != 'q') {
            char *buffer = new char[1024];
            int offset = 0;
            for (int i = 0; i < 8; i++) {
                uint8_t t;
                cin >> t;
                if (t == 'q') {
                    flag = 'q';
                    break;
                }
                cout << bitset<8>(t) << endl;
                *(buffer + offset) = t;
            }
            if (flag != 'q')
                write(sockClient, buffer, 8);
        }
    }
    else {
        cout << "bye\n";
    }
    close(sockClient);
    return 0;
}