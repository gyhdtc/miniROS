#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> a;
    cout << "a" << endl;
    a.push_back(1);
    cout << &a << endl;
    a.push_back(2);
    cout << &a << endl;
    a.push_back(3);
    for (auto& i : a)
    {
        int t;
        cin >> t;
        if (t == 0) break;
        i = t;
        cout << i << endl;
    }
    a.push_back(10);
    cout << "---" << endl;
    for (auto& i : a)
    {
        cout << i << endl;
    }
    return 0;
}