#include "../include/headv2.0.h"

class A {
private:
    int shit;
public:
    A() {
        // cout << "1\n";
    }
    A(A& a) {
        // cout << "2\n";
    }
};
void th(shared_ptr<A> a) {
    cout << "thread " << a.use_count() << endl;
    sleep(2);
    cout << "thread " << a.use_count() << endl;
}
void func() {
    shared_ptr<A> shit(new A);
    thread t1(th, shit);
    t1.detach();
    // th(shit);
    sleep(1);
    cout << "out\n";
}
int main() {
    // func();
    // sleep(3);
    uint8_t a = 0b01000110;
    if (a == 0b01000111) cout << "shit\n";
    cout << a << endl;
    return 0;
}
// class A {
// private:
//     int shit;
// public:
//     virtual void func() {
//         printf("gyh\n");
//     }
// };
// class B : public A {
// private:
//     int a;
// public:
//     virtual void func() {
//         printf("gyh2\n");
//     }
// };
// void fun(A a) {
//     a.func();
// }
// int main() {
//     // B* b = new B;
//     // B b2;
//     // fun(b2);
//     uint32_t a = 38;
//     uint8_t b = a;
//     cout << (int)b << endl;
//     return 0;
// }