#include <iostream>
#include <future>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <unistd.h>
#include <chrono>
using namespace std;

int temp = 0;
bool timeflag = false;
bool shit = false;
bool setflag = false;
mutex setflagmutex;
condition_variable cv;

void fun() {
	while (timeflag != true) {
        // sleep(5);
        unique_lock<mutex> lk1(setflagmutex);
        
        if (timeflag == false) {
            setflag = true;
            temp = 1;
            break;
        }
    }
    if (setflag == true) {
        cv.notify_one();
        shit = true;
    }
    cout << "1\n";
}
void tiktok() {
    sleep(3);
    cout << "2\n";
    unique_lock<mutex> lk(setflagmutex);
    cv.wait(lk);//, [](){ return shit == true; });
    cout << "3\n";
    if (setflag == false) {
        timeflag = true;
    }
    cout << "4\n";
}

int main()
{
	thread t1(fun);
    thread t2(tiktok);
    t1.detach();
    t2.detach();
    sleep(5);
    cout << temp << endl;
    cout << (setflag == true ? 1 : 0) << endl;
    cout << (timeflag == true ? 1 : 0)  << endl;
	return 0;
}