#ifndef _HEADER_H
#define _HEADER_H
    #include "header.h"
#endif
#ifndef _SERVER_H
#define _SERVER_H
    #include "server.h"
#endif
#ifndef _CLIENT_H
#define _CLIENT_H
    #include "client.h"
#endif

class MyThread : public Server{
    private:
        static void _CreateServer(MyThread *);
        static void _SigThread(int);
    public:
        vector<string> name;
        void CreateServer();
        void CreateSig();
        MyThread(int port, char *ip, ServerCallBack cb) : Server(port, ip, cb) {};
        MyThread(int port, string ip, ServerCallBack cb) : Server(port, ip, cb) {};
        MyThread() : Server() {};
        ~MyThread();
};

void MyThread::CreateServer() {
    thread t(_CreateServer, this);
    t.detach();
    cout << "Create Server Thread" << endl;
}
void MyThread::CreateSig() {
    signal(SIGINT, _SigThread);
    cout << "Create Sig Thread" << endl;
}
void CreateClient(int port, char *ip) {

}

void MyThread::_SigThread(int sig) {
    if (sig == SIGINT || sig == SIGSTOP)
    {
        keepRunning = 0;
        cout << endl;
    }
}
void MyThread::_CreateServer(MyThread *MT) {
    MT->ServerInit();
    MT->ServerBindIpAndPort();
    MT->WaitForConnect(MT); 
}

MyThread::~MyThread() {
    cout << "close mythread" << endl;
}