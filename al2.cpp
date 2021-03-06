#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string.h>
using namespace std;
int maxv = 1e9+7;
bool xxx(int m, int add) {
    int m1 = m/100, m2 = (m%100)/10, m3 = m%10;
    int add1 = add/100, add2 = (add%100)/10, add3 = add%10;
    if (m1 != m2 && m1 != add1 && m2 != m3 && m2 != add2 && m3 != add3) {
        if (add1 != add2 && add2 != add3) {
            return true;
        }
    }
    return false;
}

int main() {
    cout << 1 << endl;
    unordered_map<int, vector<int>> gyh;
    cout << 1 << endl;
    vector<int> shit = {121, 131, 212, 232, 313, 323, 132, 123, 231, 213, 321, 312};
    cout << 1 << endl;
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; i < 12; ++j) {
            // cout << 1 << endl;
            // gyh[shit[i]].resize(12);
            // cout << 1 << endl;
            vector<int> t;
            gyh.insert({shit[i], t});
            if (xxx(shit[i], shit[j])) gyh[shit[i]].push_back(shit[j]);
        }
    }
    for (int i = 0; i < 12; ++i) {
        cout << shit[i] << " : ";
        for (int j = 0; i < gyh[shit[i]].size(); ++j) {
            cout << gyh[shit[i]][j] << " ";
        }
        cout << endl;
    }
}