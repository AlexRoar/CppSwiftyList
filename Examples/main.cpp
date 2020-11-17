#include <stdio.h>
#include <ctime>

#include "SwiftyList.hpp"
const size_t stressElements = 10;

#define TIME_MEASURED(code){clock_t begin = clock();code;clock_t end = clock(); double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC; printf("ELAPSED: %lf sec\n", elapsed_secs);}
#define TIME_LIMIT(sec) if (double(clock() - begin) / CLOCKS_PER_SEC > sec) {printf("\tReached time limit! "); break;}

int main() {
    SwiftyList<int> list(20, 1, fopen("graphLog.html", "w"), false);

    for (size_t i = 0; i < 10; i++) {
        if (i % 2 == 1)
            list.pushBack((int)i);
        else
            list.pushFront((int)i);
    }
    list.dumpAll("Pushed elements"); // TODO: phy order
    list.optimize();
    list.dumpAll("Optimized");

    // TODO: do not trust users
    // TODO: ? prefix I_AM__SLOOOOOW_____remove(pos)
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

    list.DestructList();
    list = SwiftyList<int> (20, 0, fopen("graphLog.html", "w"), false);

    printf("Pushing %zu elements...\n", stressElements);
    TIME_MEASURED({
                      for (size_t i = 0; i < stressElements; i++) {
                          TIME_LIMIT(8)
                          if (i % 2 == 1)
                              list.pushBack((int)i);
                          else
                              list.pushFront((int)i);
                      }
                  })

    printf("Physic access on %zu elements...\n", stressElements);
    TIME_MEASURED({
                      for (size_t i = 1; i <= stressElements; i++) {
                          TIME_LIMIT(8) // TODO: time limit + counter
                          int tmp = 0;
                          list.get(i, &tmp);
                          list.set(i, tmp);
                      }
                  })


    printf("Logic access on %zu elements...\n", stressElements);
    TIME_MEASURED({
                      for (size_t i = 0; i < stressElements; i++) {
                          TIME_LIMIT(8)
                          int tmp = 0;
                          list.getLogic(i, &tmp);
                          list.setLogic(i, tmp);
                      }
                  })

    printf("Optimization...\n");
    TIME_MEASURED({
                      list.optimize();
                  })

    printf("Physic access on %zu elements...\n", stressElements);
    TIME_MEASURED({
                      for (size_t i = 1; i <= stressElements; i++) {
                          TIME_LIMIT(8)
                          int tmp = 0;
                          list.get(i, &tmp);
                          list.set(i, tmp);
                      }
                  })


    printf("Logic access on %zu elements...\n", stressElements);
    TIME_MEASURED({
                      for (size_t i = 0; i < stressElements; i++) {
                          TIME_LIMIT(8)
                          int tmp = 0;
                          list.getLogic(i, &tmp);
                          list.setLogic(i, tmp);
                      }
                  })


    return 0;
}
