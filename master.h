#ifndef A
#define A
#include "header.h"
#endif
#include "server.h"
class Master : public Server {
    private:
        vector<Node> nodes;

    public:
        Master(int port, char *ip, ServerCallBack cb) : Server(port, ip, cb) {};
        ~Master();
        void PushName(string);
        void GetName(int, string&);
        int GetNameLen();
};

Master::Master() {
    cout << "master start!\n";
}

Master::~Master() {
    cout << "master stop!\n";
}

void Master::PushName(string s) {
    Node t;
    t.name = s;
    nodes.push_back(t);
}

void Master::GetName(int i, string &name) {
    if (i > 0 && i <= nodes.size())
    {
        name = nodes[i-1].name;
        return;
    }
    name = "";
    return;
}

int Master::GetNameLen() {
    return nodes.size();
}