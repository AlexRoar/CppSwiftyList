#include <stdio.h>
#include <ctime>

#include "SwiftyList.hpp"

#define TIME_MEASURED(code){clock_t begin = clock();code;clock_t end = clock();\
double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;\
printf("ELAPSED: %lf sec\n", elapsed_secs);}

int main() {
    SwiftyList<int> list(0, 2, fopen("graphLog.html", "w"), false);

    for (size_t i = 0; i < 10; i++) {
        if (i % 2 == 1)
            list.pushBack(i);
        else
            list.pushFront(i);
    }
    list.dumpAll("Pushed elements");
    list.optimize();
    list.dumpAll("Optimized");

    return 0;
}
