#include "../include/server_head.h"
#include "../include/server_thread.h"
#include "../include/server_class.h"
#include "../include/server_class_define.h"

int main() {
    Broke* mybroke = new Broke;
    mybroke->StartServer(IPADDRESS, PORT);
    signal(SIGINT, SigThread);
    while (KeepRunning);
    delete mybroke;
    return 0;
}