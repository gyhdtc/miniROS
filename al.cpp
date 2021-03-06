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
// 初始位置的地铁线路编号     num
//                终点     t
//            线路数量     cnt
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
        int ditienum, s, t;
        cin >> ditienum >> s >> t;
        vector<int> res;
        for (int i = 0; i < ditienum; ++i) {
            int k = 0, a;
            cin >> k;
            for (int j = 0; j < k; ++j) {
                // 一堆站台编号
                cin >> a;
                if (a == s) { // 记录初始位置所在的地铁线
                    res.push_back(i);
                }
                um1[i].insert(a); // 地铁线路 --- 站台s
                um2[a].insert(i); // 站台 --- 地铁线路s
            }
        }
        // 遍历地铁线
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