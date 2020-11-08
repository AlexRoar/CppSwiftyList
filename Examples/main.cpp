#include <stdio.h>
#include <ctime>

#include "SwiftyList.hpp"

#define TIME_MEASURED(code){clock_t begin = clock();code;clock_t end = clock();\
double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;\
printf("ELAPSED: %lf sec\n", elapsed_secs);}

int main() {
    SwiftyList<int> list(20, 2, fopen("graphLog.html", "w"), false);

    for (size_t i = 0; i < 10; i++) {
        if (i % 2 == 1)
            list.pushBack(i);
        else
            list.pushFront(i);
    }
    list.dumpAll("Pushed elements");
    list.optimize();
    list.dumpAll("Optimized");

    list.remove(1);
    list.remove(5);
    list.remove(8);

    list.dumpAll("Removed some elements");

    list.remove(8);
    list.remove(5);

    int tmp = 4;
    list.get(5, &tmp);
    list.set(5, tmp);

    list.dumpAll("Tried accessing invalid elements");

    list.pushBack(101);
    list.pushBack(102);

    list.dumpAll("Pushed some elements");

    list.pushBack(103);
    list.pushBack(104);

    list.dumpAll("Even more");

    list.clear();

    list.dumpAll("Cleared");

    list.pushBack(103);
    list.pushBack(104);

    list.dumpAll("Pushed again");
    
    list.destructList();

    return 0;
}
