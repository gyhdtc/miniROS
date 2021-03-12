#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
using namespace std;
int main() {
    int n;
    cin >> n;
    vector<int> num;
    for (int i = 0; i < n; ++i) {
        int t;
        cin >> t;
        num.push_back(t);
    }
    if (n < 3) {
        cout << -1 << endl;
        return 0;
    }
    
}