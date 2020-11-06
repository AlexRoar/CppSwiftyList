#include <stdio.h>
#include <ctime>

#include "SwiftyList.hpp"

#define TIME_MEASURED(code){clock_t begin = clock();code;clock_t end = clock();\
double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;\
printf("ELAPSED: %lf sec\n", elapsed_secs);}

int main() {
    SwiftyList<int> list(0, 2, fopen("graphLog.html", "w"), false);

    list.dumpAll("Pushing elements...");
    size_t physPos = 0;
    for (size_t i = 0; i < 10; i++) {
        list.pushBack(i);
    }
    list.dumpAll("Pushed");
    list.removeLogic(0);
    int val = 0;
    list.getLogic(0, &val);
    list.dumpAll("Removed");

    return 0;
}
