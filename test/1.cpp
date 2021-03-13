#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
using namespace std;

struct Student { 
    int64_t id;
    std::string name;
    int64_t age;
};

template <typename T>
struct listnode
{
    T val;
    listnode<T>* next;
    listnode(T stu) : val(stu), next(NULL) {}
    listnode() : val({-1, "", -1}), next(NULL) {}
};

template <typename T>
class mylist
{
private:
    int len;
    listnode<T>* head;
    listnode<T>* tail;
public:
    mylist() : head(NULL), tail(NULL), len(0) {};
    mylist(const mylist<T>& ml);

    int size() {return len;}
    void push_back(listnode<T>);
    void push_front(listnode<T>);
    listnode<T> pop_back();
    listnode<T> pop_front();
    void clear();
    bool empty() {return len == 0;}
    void reverse();
    
    void printlist();

    ~mylist() {
        while (head != NULL) {
            listnode<T>* t = head;
            head = head->next;
            delete t;
        }
    }
};
template <typename T>
void mylist<T>::clear() {
    while (head != NULL) {
        listnode<T>* t = head;
        head = head->next;
        delete t;
    }
    len = 0;
}

template <typename T>
mylist<T>::mylist(const mylist<T>& ml) {
    len = ml.len;
    head = new listnode<T>;
    listnode<T>* t = ml.head;
    listnode<T>* newlist = head;
    while (t != NULL) {
        *newlist = *t;
        t = t->next;
        if (t != NULL) {
            newlist->next = new listnode<T>;
            tail = newlist;
            newlist = newlist->next;
        }
    }
}

template <typename T>
void mylist<T>::printlist() {
    listnode<T>* t = head;
    while (t != NULL) {
        cout << t->val.id << " ";
        t = t->next;
    }
    cout << endl;
}
template <typename T>
void mylist<T>::push_back(listnode<T> stu) {
    listnode<T>* t = new listnode<T>(stu);
    if (len == 0) {
        head = t;
        tail = head;
    }
    else {
        tail->next = t;
        tail = t;
    }
    len++;
}
template <typename T>
void mylist<T>::push_front(listnode<T> stu) {
    listnode<T>* t = new listnode<T>(stu);
    if (len == 0) {
        head = t;
        tail = head;
    }
    else {
        listnode<T>* oldhead = head;
        head = t;
        t->next = oldhead;
    }
    len++;
}
template <typename T>
listnode<T> mylist<T>::pop_front() {
    listnode<T> res;
    if (len == 0) {
        cout << "empty!\n";
        return res;
    }
    else {
        res = *head;
        listnode<T>* t = head;
        head = head->next;
        len--;
        delete t;
    }
    return res;
}
template <typename T>
listnode<T> mylist<T>::pop_back() {
    listnode<T> res;
    if (len == 0) {
        cout << "empty!\n";
        return res;
    }
    else {
        res = *tail;
        listnode<T>* t1 = head;
        listnode<T>* t2 = head->next;
        while (t2 != NULL && t2 != tail) {
            t1= t2;
            t2 = t2->next;
        }
        tail = t1;
        tail->next = NULL;
        len--;
        if (t2 != NULL) delete t2;
    }
    return res;
}
template <typename T>
void mylist<T>::reverse() {
    if (len == 0) {
        cout << "empty!\n";
        return;
    }
    else {
        listnode<T>* t1 = head;
        listnode<T>* t2 = head->next;
        while (t2 != NULL) {
            t1->next = t2->next;
            t2->next = head;
            head = t2;
            t2 = t1->next;
        }
    }
}
int main() {
    mylist<Student> list;
    list.push_back(Student{ 1, "hello", 24 });
    list.push_back(Student{ 2, "world", 27 });
    list.push_back(Student{ 3, "bob", 29 });
    list.push_front(Student{ 4, "jason", 0 });
    list.push_front(Student{ 5, "foo", 20 });

    // list.printlist();

    assert(5 == list.size());
    assert(!list.empty());
    listnode<Student> a = list.pop_front();
    assert(4 == list.size());
    listnode<Student> b = list.pop_back();
    assert(3 == list.size());
    list.reverse();

    // list.printlist();

    mylist<Student> copy_list1 = list;

    // copy_list1.printlist();

    mylist<Student> copy_list2{list};

    // copy_list2.printlist();

    assert(3 == copy_list1.size());
    copy_list1.clear();

    // copy_list2.printlist();

    assert(0 == copy_list1.size());
    assert(3 == list.size());
    copy_list2.clear();
}