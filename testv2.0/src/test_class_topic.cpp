#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <errno.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory.h>
#include <signal.h>
#include <bitset>
#include <stdarg.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <atomic>
#include <set>
using namespace std;

class Topic
{
private:
    map<int32_t, set<string>> sub2name; // 发布节点序号 --- 发布话题名称s
    map<int32_t, set<string>> pub2name; // 发布节点序号 --- 订阅话题名称s
    map<string, pair<int32_t, int32_t>> name2subANDpub; // 名字 --- （订阅节点，发布节点）
public:
    void subTopic(string, int32_t); // 某节点发布话题
    void pubTopic(string, int32_t); // 某节点订阅话题
    void delTopic(string, int32_t); // 删除某节点 订阅/发布 的话题
};
void Topic::subTopic(string name, int32_t index) {
    if (name2subANDpub.find(name) == name2subANDpub.end() || (name2subANDpub.find(name) != name2subANDpub.end() && name2subANDpub[name].first == 0)) {
        // 没有这个话题，或者有这个话题但是没有话题发布者
        if (name2subANDpub.find(name) == name2subANDpub.end()) {
            // 没有这个话题
            name2subANDpub[name] = {index, 0};
        }
        else {
            // 有这个话题
            name2subANDpub[name].first = index;
        }
        if (sub2name.find(index) == sub2name.end()) {
            sub2name.insert(pair<int32_t, set<string>>{index, set<string>{name}});
        }
        else {
            sub2name[index].insert(name);
        }
    }
    else {
        // 有这个话题，并且有话题发布者
        printf("%s already subed by %d\n", name.c_str(), index);
    }
}
void Topic::pubTopic(string name, int32_t index) {
    if (name2subANDpub.find(name) == name2subANDpub.end() || (name2subANDpub.find(name) != name2subANDpub.end() && (name2subANDpub[name].second & index) == 0)) {
        // 没有这个话题，或者有这个话题但是此节点没有订阅此话题
        if (name2subANDpub.find(name) == name2subANDpub.end()) {
            // 没有这个话题
            name2subANDpub[name] = {0, index};
        }
        else {
            // 有这个话题
            name2subANDpub[name].second = name2subANDpub[name].second | index;
        }
        if (pub2name.find(index) == pub2name.end()) {
            pub2name.insert(pair<int32_t, set<string>>{index, set<string>{name}});
        }
        else {
            pub2name[index].insert(name);
        }
    }
    else {
        // 有这个话题，并且有话题发布者
        printf("%s already subed by %d\n", name.c_str(), index);
    }
}
void Topic::delTopic(string name, int32_t index) {
    // 删除全部
    if (name == "all") {
        vector<string> tname;
        if (sub2name.find(index) != sub2name.end()) {
            for (auto i : sub2name[index]) {
                tname.push_back(i);
            }
            sub2name.erase(index);
        }
        if (pub2name.find(index) != pub2name.end()) {
            for (auto i : pub2name[index]) {
                tname.push_back(i);
            }
            pub2name.erase(index);
        }
        for (auto topicname : tname) {
            name2subANDpub[topicname].first &= (0xffffffff ^ index);
            name2subANDpub[topicname].second &= (0xffffffff ^ index);
            if (name2subANDpub[topicname].first == 0 && name2subANDpub[topicname].second == 0) {
                name2subANDpub.erase(topicname);
            }
        }
    }
    // 删除某一个
    else {
        if (sub2name[index].find(name) != sub2name[index].end()) {
            sub2name[index].erase(name);
        }
        if (pub2name[index].find(name) != pub2name[index].end()) {
            pub2name[index].erase(name);
        }
        name2subANDpub[name].first &= (0xffffffff ^ index);
        name2subANDpub[name].second &= (0xffffffff ^ index);
    }
}
