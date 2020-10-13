#ifndef A
#define A
#include "header.h"
#endif

struct Node {
    int port;
    string ip;
    string name;
    vector<string> sub_list;
    vector<string> put_list;
};

class Master {
    private:
        vector<Node> nodes;
    public:
        Master();
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