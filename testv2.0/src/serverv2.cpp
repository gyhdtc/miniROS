#include "../include/server_head.h"
#include "../include/server_thread.h"
#include "../include/server_class.h"
#include "../include/server_class_define.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        IPADDRESS = argv[1];
    }
    if (argc == 3) {
        IPADDRESS = argv[1];
        PORT = stoi(string(argv[2]));
    }
    Broke* mybroke = new Broke;
    mybroke->StartServer(IPADDRESS, PORT);
    signal(SIGINT, SigThread);
    while (KeepRunning);
    delete mybroke;
    return 0;
}