#include "../include/headv2.0.h"

class A {
private:
    int shit;
public:
    A() {
        cout << "1\n";
    }
    A(A& a) {
        cout << "2\n";
    }
};
void th(shared_ptr<A>& a) {
    cout << a.use_count() << endl;
    sleep(2);
    cout << a.use_count() << endl;
}
void func() {
    shared_ptr<A> shit(new A);
    // thread t1(th, shit);
    // t1.detach();
    th(shit);
    sleep(1);
}
int main() {
    func();
    sleep(5);
    return 0;
}