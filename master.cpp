#include "master.h"
#include "header.h"

Master::Master() {
    cout << "master start!\n";
}

Master::~Master() {
    cout << "master stop!\n";
}

void Master::PushName(string s) {
    node.push_back(s);
}

void Master::GetName(int i, string &name) {
    if (i > 0 && i <= node.size())
    {
        name = node[i-1];
        return;
    }
    name = "";
    return;
}

int Master::GetNameLen() {
    return node.size();
}