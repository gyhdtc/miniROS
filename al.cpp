#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string.h>
using namespace std;
int maxv = 10000000;
unordered_map<int, unordered_set<int>> um1;
unordered_map<int, unordered_set<int>> um2;
int minv = 10000000;
bool vis[10000001] = {false};
void dfs(int num, int t, int cnt) {
    if (cnt >= minv) {
        return ;
    }
    if (um1[num].find(t) != um1[num].end()) {
        minv = min(minv, cnt);
        return;
    }
    vis[num] = true;
    for (int i : um1[num]) {
        if (um2[i].size() > 1) {
            for (int p : um2[i]) {
                if (!vis[p]) {
                    dfs(p, t, cnt+1);
                }
            }
        }
    }
    vis[num] = false;
}
int main() {
    int t = 0;
    cin >> t;
    while (t--) {
        int n, s, t;
        cin >> n >> s >> t;
        vector<int> res;
        for (int i = 0; i < n; ++i) {
            int k = 0, a;
            cin >> k;
            for (int j = 0; j < k; ++j) {
                cin >> a;
                if (a == s) {
                    res.push_back(i);
                }
                um1[i].insert(a);
                um2[a].insert(i);
            }
        }
        for (int y : res) {
            dfs(y, t, 1);
        }
        if (minv == maxv) {
            cout << -1 << endl;
        }
        else {
            cout << minv << endl;
        }
        res.clear();
        minv = maxv;
        memset(vis, 0, sizeof(vis));
        um1.clear();
        um2.clear();
    }
    return 0;
}