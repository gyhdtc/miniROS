#include <iostream>
#include <thread>
#include <vector>
#include <stdio.h>
using namespace std;
 
class Position {
public:
    Position(int xx, int yy) : _x(xx), _y(yy) {};
    int& x(){ return _x; }
    int& y(){ return _y; }
 
    void member_func(int i) {
        printf("member_func:%d\n", i);
        _x = 100;
    }
 
    friend void pointer_style(Position *p, int i);
    friend void reference_style(Position &p, int i);
 
private:
    int _x;
    int _y;
};
 
//! 友元函数，通过指针来传递类的实例
void pointer_style(Position *p, int i) {
    printf("pointer_style:%d(%d,%d)\n", i, p->_x, p->_y);
    p->_x = 200;
}
 
//! 友元函数，通过引用来访问类的实例
void reference_style(Position &p, int i) {
    printf("reference_style:%d(%d,%d)\n", i, p._x, p._y);
    p._x = 300;
}
 
int main() {
    Position p(1, 2);
 
    /**
     * 1、使用类的成员函数作为子线程的参数
     * 参数说明：//参数1是成员函数的指针，参数2是Position对象值传递,
     * 子线程中对p的修改不会影响主线程中的对象p。
     */
 
    thread t1(&Position::member_func, p, 233);
    t1.join();
    cout << "p.x=" << p.x() << endl;
 
    /**
     * 2、使用类的友元函数作为参数
     * 友元函数参见：https://blog.csdn.net/mary19920410/article/details/70036878
     * 参数说明：参数1是Position的友元函数reference_style，参数2是用ref包装过的引用，
     * 参数3是传给reference_style函数的值。
     * 在这种方式中，由于p是以引用的方式传递给子线程，在子线程中对p的修改相当于修改主线程中的p。
     */
    std::thread t2(reference_style, ref(p), 1);
    t2.join();
    cout << "p.x=" << p.x() << endl;
 
    /**
     * 3、使用类的友元函数作为参数
     * 参数说明：参数1是Position的友元函数pointer_style，参数2是p的指针，参数3是传给
     * pointer_style的参数。
     * 在这种方式中，由于p是以引用的方式传递给子线程，在子线程中对p的修改相当于修改主线程中的p。
     */
    std::thread t3(pointer_style, &p, 1);
    t3.join();
    cout << "p.x=" << p.x() << endl;
 
    /**
     * 4、通过lambda传递引用参数
     * 关于lambda函数的说明:https://blog.csdn.net/u010984552/article/details/53634513
     */
    auto lambda_func = [&p](int i) -> void {
        printf("lambda_func:%d(%d,%d)\n", i, p.x(), p.y());
        p.x() = 400;
    };
    /**
     * 参数说明：参数1是lambda表达式,参数2是传递给lambda表达式的参数
     * 由于在lambda_func是以引用的方式捕获p，所以在子线程中p是主线程中p的引用，在子线程中修改p
     * 相当于主线程修改主县城中的p。也可以以值传递的方式捕获p，这样在子线程中修改p就不会影响到主
     * 线程中的p。
     */
    thread t4(lambda_func, 123);
    t4.join();
    cout << "p.x=" << p.x() << endl;
    return 0;
}