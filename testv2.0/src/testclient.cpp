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
            uint8_t buffer[20] = {0};
            char c;
            cin >> c;
            // heartbeat
            if (c == '0') {
                buffer[0] = 0b00000001;
                write(sockClient, buffer, 8);
                continue;
            }
            // reg
            if (c == '1') {
                buffer[0] = 0b00010000;
                string name;
                cout << "input name\n";
                cin >> name;
                buffer[3] = name.size();
                buffer[4] = codeGenera(name.c_str(), name.size());
                memcpy(buffer+8, name.c_str(), name.size());
                write(sockClient, buffer, 8+name.size());
                continue;
            }
            // sub
            if (c == '2') {
                buffer[0] = 0b00001000;
                string name;
                cout << "input sub name\n";
                cin >> name;
                int index;
                cout << "input node index\n";
                cin >> index;
                buffer[1] = index;
                buffer[2] = name.size();
                buffer[4] = codeGenera(name.c_str(), name.size());
                memcpy(buffer+8, name.c_str(), name.size());
                write(sockClient, buffer, 8+name.size());
                continue;
            }
            // pub
            if (c == '3') {
                buffer[0] = 0b00000100;
                string name;
                cout << "input pub name\n";
                cin >> name;
                int index;
                cout << "input node index\n";
                cin >> index;
                buffer[1] = index;
                buffer[2] = name.size();
                buffer[4] = codeGenera(name.c_str(), name.size());
                memcpy(buffer+8, name.c_str(), name.size());
                write(sockClient, buffer, 8+name.size());
                continue;
            }
            // del
            if (c == '4') {
                buffer[0] = 0b00001100;
                string name;
                cout << "input del name\n";
                cin >> name;
                int index;
                cout << "input node index\n";
                cin >> index;
                buffer[1] = index;
                buffer[2] = name.size();
                buffer[4] = codeGenera(name.c_str(), name.size());
                memcpy(buffer+8, name.c_str(), name.size());
                write(sockClient, buffer, 8+name.size());
                continue;
            }
            if (c == 'q') break;
        }
    }
    else {
        cout << "bye\n";
    }
    close(sockClient);
    return 0;
}