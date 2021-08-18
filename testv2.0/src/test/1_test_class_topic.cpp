#include <thread>
#include <stdio.h>
//#include <unistd.h>
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
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <sys/epoll.h>
#include <atomic>
#include <set>
#include <ctime>
#include <random>

using namespace std;

class Topic
{
private:
	map<uint32_t, set<string>> sub2name; // 发布节点序号 --- 发布话题名称s
	map<uint32_t, set<string>> pub2name; // 发布节点序号 --- 订阅话题名称s
	map<string, pair<uint32_t, uint32_t>> name2subANDpub; // 名字 --- （订阅节点，发布节点）
public:
	void subTopic(string, uint32_t); // 某节点发布话题
	void pubTopic(string, uint32_t); // 某节点订阅话题
	void delTopic(string, uint32_t); // 删除某节点 订阅/发布 的话题
	void printName(string);// 打印话题信息
	void printIndex(uint32_t);// 打印节点信息
};
void Topic::subTopic(string name, uint32_t index) {
	if (name2subANDpub.find(name) == name2subANDpub.end() || (name2subANDpub.find(name) != name2subANDpub.end() && name2subANDpub[name].first == 0)) {
		// 没有这个话题，或者有这个话题但是没有话题发布者
		if (name2subANDpub.find(name) == name2subANDpub.end()) {
			// 没有这个话题
			name2subANDpub[name] = { index, 0 };
		}
		else {
			// 有这个话题
			name2subANDpub[name].first = index;
		}
		if (sub2name.find(index) == sub2name.end()) {
			sub2name.insert(pair<uint32_t, set<string>>{index, set<string>{name}});
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
void Topic::pubTopic(string name, uint32_t index) {
	if (name2subANDpub.find(name) == name2subANDpub.end() || (name2subANDpub.find(name) != name2subANDpub.end() && (name2subANDpub[name].second & index) == 0)) {
		// 没有这个话题，或者有这个话题但是此节点没有订阅此话题
		if (name2subANDpub.find(name) == name2subANDpub.end()) {
			// 没有这个话题
			name2subANDpub[name] = { 0, index };
		}
		else {
			// 有这个话题
			name2subANDpub[name].second = name2subANDpub[name].second | index;
		}
		if (pub2name.find(index) == pub2name.end()) {
			pub2name.insert(pair<uint32_t, set<string>>{index, set<string>{name}});
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
void Topic::delTopic(string name, uint32_t index) {
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



void Topic::printName(string name) {
	cout << "----------------话题信息-------------------\n";
	if (name2subANDpub.find(name) == name2subANDpub.end()) {
		cout << "没有" << name << "话题的相关信息\n";
	}
	else {
		// 输出为二进制
		auto idTobit = [](uint32_t index)->string {
			string res = "0000 0000 0000 0000 0000 0000 0000 0000";
			int i = 0;
			while (index) {
				if (index & 1) {
					res[38 - (i + i / 4)] = '1';
				}
				i++;
				index = index >> 1;
			}
			return res;
		};
		// 返回1的个数
		auto numOf1 = [](uint32_t num)->int {
			int res = 0;
			while (num) {
				res++;
				num = num & (num - 1);
			}
			return res;
		};
		cout << name << "话题的发布者：\n";
		if (numOf1(name2subANDpub[name].first) != 1) {
			cout << "此话题没有发布者！\n";
		}
		else {
			cout << idTobit(name2subANDpub[name].first) << "\n";
		}
		cout << name << "话题的订阅者：\n";
		if (name2subANDpub[name].first == 0) {
			cout << "此话题还没有订阅者！\n";
		}
		else {
			cout << idTobit(name2subANDpub[name].second) << "\n";
		}
	}
	cout << "-----------------------------------------\n";
}
void Topic::printIndex(uint32_t index) {
	// 输出为二进制
	auto idTobit = [](uint32_t index)->string {
		string res = "0000 0000 0000 0000 0000 0000 0000 0000";
		int i = 0;
		while (index) {
			if (index & 1) {
				res[38 - (i + i / 4)] = '1';
			}
			i++;
			index = index >> 1;
		}
		return res;
	};
	string myid = idTobit(index);
	cout << "----------------节点信息-------------------\n";
	if (sub2name.find(index) == sub2name.end()) {
		cout << "节点" << myid << "没有发布话题！\n";
	}
	else {
		cout << "节点" << myid << "发布的话题：\n";
		for (auto name : sub2name[index]) {
			cout << name << ", ";
		}
		cout << "\n";
	}
	if (pub2name.find(index) == pub2name.end()) {
		cout << "节点" << myid << "没有订阅话题！\n";
	}
	else {
		cout << "节点" << myid << "订阅的话题：\n";
		for (auto name : pub2name[index]) {
			cout << name << ", ";
		}
		cout << "\n";
	}
	cout << "-----------------------------------------\n";
}


int main() {
	srand(time(0));
	vector<uint32_t> id(32);
	for (int i = 0; i < 32; ++i) {
		id[i] = 1 << i;
	}
	auto nameRand = [](int n)->string {
		string name;
		for (int i = 0; i < n; ++i) {
			int t = rand() % 36;
			if (t < 10) {
				name += char(t + '0');
			}
			else {
				t -= 10;
				name += char(t + 'A');
			}
		}
		return name;
	};
	vector<string> s;
	for (int i = 0; i < 20; ++i) {
		s.push_back((string)nameRand(5));
	}
	Topic topic;
	auto actions = [&](Topic topic, string name, uint32_t index, int ac)->void{
		if (ac == 1) {
			topic.subTopic(name, index);
		}
		if (ac == 2) {
			topic.pubTopic(name, index);
		}
		if (ac == 0) {
			topic.delTopic(name, index);
		}
		topic.printName(name);
		topic.printIndex(index);
	};
	auto numTobit = [](int x)->string {
		string res = "0000 0000 0000 0000 0000 0000 0000 0000";
		res[38 - (x + x / 4)] = '1';
		return res;
	};
	for (int i = 0; i < 6; ++i) {
		int ac = rand() % 3;
		int indexid = rand() % 32;
		int nameid = rand() % 20;
		//cout << i + 1 << "次操作： " << s[name] << "； " << index << "->" << id[index] << "；" << ac << "（1：发布话题；2：订阅话题；0：删除）\n";
		cout << "第" << i + 1 << "次操作：" << indexid + 1 << "->" << numTobit(indexid) << " ：" << s[nameid];
		string ss = (ac == 1) ? "发布" : ((ac == 2) ? "订阅" : "删除");
		cout << ss << "\n";
		actions(topic, s[nameid], id[indexid], ac);
	}
	return 0;
}