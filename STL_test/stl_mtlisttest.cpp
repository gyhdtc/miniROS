#include "stl_mylistiter.h"

int main() {
    List<int> mylist;
    for (int i=0; i < 5; ++i) {
        mylist.insert_front(i);
        mylist.insert_end(i+2);
    }
}