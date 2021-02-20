#include <iostream>
#include <thread>
using namespace std;

class shittest
{
public:
    /* data */
    int* x;
    string name;
public:
    shittest(string);
    shittest(const shittest& gyh)
    {
        x = gyh.x;
        name = gyh.name;
    }
    ~shittest();
};

shittest::shittest(string s)
{
    name = s;
    cout << name << " - s" << endl;
    x = new int(2);
}

shittest::~shittest()
{
    cout << name << " - c" << endl;
    cout << "ccc" << endl;
    delete x;
}

void func(shittest h)
{
    cout << 3 << endl;
    // shittest m = h;
    cout << 4 << endl;
    cout << "func " << h.x << endl;
    // delete m;
    // delete h;
    cout << 5 << endl;
    return;
}

int main()
{
    while (1)
    {
        cout << 0 << endl;
        int a = 1;
        shittest* b = new shittest("b");
        // b->x = &a;
        cout << 1 << endl;
        func(*b);
        cout << 2 << endl;
        // cout << *b->x << endl;
        // delete b;
        cin >> a;
    }
    return 0;
}