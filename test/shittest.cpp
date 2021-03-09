#include <iostream>
#include <thread>
#include <ctype.h>
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
    uint8_t t[4] = {1,2,3,4};
    uint32_t a = uint8_t(t);
    cout << a << endl;
    return 0;
}