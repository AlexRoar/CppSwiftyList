#include <stdio.h>
#include <ctime>

#include "SwiftyList.hpp"

#define TIME_MEASURED(code){clock_t begin = clock();code;clock_t end = clock();\
double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;\
printf("ELAPSED: %lf sec\n", elapsed_secs);}

int main() {
    SwiftyList<int> list(1, 2, fopen("graphLog.html", "w"), false);

    const size_t nElem = 10;
    printf("Pushing %zu elements...\n", nElem);
    TIME_MEASURED({
                      for (size_t i = 0; i < nElem; ++i) {
                          list.dumpAll("Pushing elements...");
                          if (i % 2 == 0)
                              list.pushBack(i);
                          else
                              list.pushFront(i);
                      }
                  })
    list.deOptimize();
    list.dumpAll("After de-optimization");

    if (nElem < 100) {
        printf("\nSet-get operation on %zu elements ...\n", nElem);
        TIME_MEASURED({
                          for (size_t i = 0; i < nElem; ++i) {
                              int value = 0;
                              list.get(i, &value);
                              list.set(i, value);
                          }
                      })
    }

    printf("\nOptimization ...\n");
    TIME_MEASURED({
                      list.optimize();
                  })
    list.dumpAll("After optimization");


    printf("\nSet-get operation on %zu elements ...\n", nElem);
    TIME_MEASURED({
                      for (size_t i = 0; i < nElem; ++i) {
                          int value = 0;
                          list.get(i, &value);
                          list.set(i, value);
                      }
                  })

    list.remove(1);
    list.remove(3);
    list.remove(7);
    list.dumpAll("Removed some elements");

    printf("\nOptimization ...\n");
    TIME_MEASURED({
                      list.optimize();
                  })
    list.dumpAll("After optimization");

    printf("\nClear ...\n");
    TIME_MEASURED({
                      list.clear();
                  })
    list.dumpAll("Cleared");


    return 0;
}
