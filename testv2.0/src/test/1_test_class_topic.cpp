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
	map<uint32_t, set<string>> sub2name; // �����ڵ���� --- ������������s
	map<uint32_t, set<string>> pub2name; // �����ڵ���� --- ���Ļ�������s
	map<string, pair<uint32_t, uint32_t>> name2subANDpub; // ���� --- �����Ľڵ㣬�����ڵ㣩
public:
	void subTopic(string, uint32_t); // ĳ�ڵ㷢������
	void pubTopic(string, uint32_t); // ĳ�ڵ㶩�Ļ���
	void delTopic(string, uint32_t); // ɾ��ĳ�ڵ� ����/���� �Ļ���
	void printName(string);// ��ӡ������Ϣ
	void printIndex(uint32_t);// ��ӡ�ڵ���Ϣ
};
void Topic::subTopic(string name, uint32_t index) {
	if (name2subANDpub.find(name) == name2subANDpub.end() || (name2subANDpub.find(name) != name2subANDpub.end() && name2subANDpub[name].first == 0)) {
		// û��������⣬������������⵫��û�л��ⷢ����
		if (name2subANDpub.find(name) == name2subANDpub.end()) {
			// û���������
			name2subANDpub[name] = { index, 0 };
		}
		else {
			// ���������
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
		// ��������⣬�����л��ⷢ����
		printf("%s already subed by %d\n", name.c_str(), index);
	}
}
void Topic::pubTopic(string name, uint32_t index) {
	if (name2subANDpub.find(name) == name2subANDpub.end() || (name2subANDpub.find(name) != name2subANDpub.end() && (name2subANDpub[name].second & index) == 0)) {
		// û��������⣬������������⵫�Ǵ˽ڵ�û�ж��Ĵ˻���
		if (name2subANDpub.find(name) == name2subANDpub.end()) {
			// û���������
			name2subANDpub[name] = { 0, index };
		}
		else {
			// ���������
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
		// ��������⣬�����л��ⷢ����
		printf("%s already subed by %d\n", name.c_str(), index);
	}
}
void Topic::delTopic(string name, uint32_t index) {
	// ɾ��ȫ��
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
	// ɾ��ĳһ��
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
	cout << "----------------������Ϣ-------------------\n";
	if (name2subANDpub.find(name) == name2subANDpub.end()) {
		cout << "û��" << name << "����������Ϣ\n";
	}
	else {
		// ���Ϊ������
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
		// ����1�ĸ���
		auto numOf1 = [](uint32_t num)->int {
			int res = 0;
			while (num) {
				res++;
				num = num & (num - 1);
			}
			return res;
		};
		cout << name << "����ķ����ߣ�\n";
		if (numOf1(name2subANDpub[name].first) != 1) {
			cout << "�˻���û�з����ߣ�\n";
		}
		else {
			cout << idTobit(name2subANDpub[name].first) << "\n";
		}
		cout << name << "����Ķ����ߣ�\n";
		if (name2subANDpub[name].first == 0) {
			cout << "�˻��⻹û�ж����ߣ�\n";
		}
		else {
			cout << idTobit(name2subANDpub[name].second) << "\n";
		}
	}
	cout << "-----------------------------------------\n";
}
void Topic::printIndex(uint32_t index) {
	// ���Ϊ������
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
	cout << "----------------�ڵ���Ϣ-------------------\n";
	if (sub2name.find(index) == sub2name.end()) {
		cout << "�ڵ�" << myid << "û�з������⣡\n";
	}
	else {
		cout << "�ڵ�" << myid << "�����Ļ��⣺\n";
		for (auto name : sub2name[index]) {
			cout << name << ", ";
		}
		cout << "\n";
	}
	if (pub2name.find(index) == pub2name.end()) {
		cout << "�ڵ�" << myid << "û�ж��Ļ��⣡\n";
	}
	else {
		cout << "�ڵ�" << myid << "���ĵĻ��⣺\n";
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
		//cout << i + 1 << "�β����� " << s[name] << "�� " << index << "->" << id[index] << "��" << ac << "��1���������⣻2�����Ļ��⣻0��ɾ����\n";
		cout << "��" << i + 1 << "�β�����" << indexid + 1 << "->" << numTobit(indexid) << " ��" << s[nameid];
		string ss = (ac == 1) ? "����" : ((ac == 2) ? "����" : "ɾ��");
		cout << ss << "\n";
		actions(topic, s[nameid], id[indexid], ac);
	}
	return 0;
}