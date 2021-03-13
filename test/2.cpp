#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <set>
using namespace std;

int middle(vector<int>& a, int start, int end) {
    int i = start;
    int j = end;
    int temp = a[i];
    while (i < j){
        while (a[j] >= temp && i < j) --j;
        a[i] = a[j];
        while (a[i] < temp && i < j) ++i;
        a[j] = a[i];
    }
    a[i] = temp;
    return i;
}

void quicksort(vector<int>& a, int start, int end) {
    if (start == end) return;
    int i = middle(a, start, end);
    if (start <= end) {
        quicksort(a, start, i);
        quicksort(a, i+1, end);
    }
}

void sort(vector<int>& a) {
    int end = a.size();
    if (end <= 0) return;
    quicksort(a, 0, end-1);
}
void printvector(const vector<int>& a) {
    for (int i = 0; i < a.size(); ++i) cout << a[i] << " ";
    cout << endl;
}
vector<int> uniq(vector<int> a) {
    vector<int> res;
    set<int> s;
    int size = a.size();
    if (size < 0) return res;
    for (int i = 0; i < size; ++i) {
        if (s.count(a[i]) == 0) {
            res.push_back(a[i]);
            s.insert(a[i]);
        }
    }
    return res;
}
int main() {
    vector<int> a = {1,3,5,8,3};
    vector<int> b = {1,3,5,8,3};
    sort(a);
    printvector(a);
    vector<int> c = uniq(b);
    printvector(c);
}