#include <stdio.h>
#include <ctime>

#include "SwiftyList.hpp"

#define TIME_MEASURED(code){clock_t begin = clock();code;clock_t end = clock();\
double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;\
printf("ELAPSED: %lf sec\n", elapsed_secs);}

int main() {
    SwiftyList* list = new SwiftyList(0, 2, NULL, NULL, false);
    ListGraphDumper* dumper = new ListGraphDumper(list, (char*)"grapfStructure.gv");
    
    const size_t nElem = 500;
    printf("Pushing %zu elements...\n", nElem);
    TIME_MEASURED({
        for (size_t i = 0; i < nElem; i++) {
            if (i%2 == 0)
                list->pushBack(rand());
            else
                list->pushFront(rand());
        }
    })
    
//    printf("\nSet-get operation on %zu elements ...\n", nElem);
//    TIME_MEASURED({
//        for (size_t i = 0; i < nElem; i++) {
//            int value = 0;
//            list->get(i, &value);
//            list->set(i, value);
//        }
//    })
    
//    printf("\nDeoptimizing ...\n");
//    TIME_MEASURED({
//        list->deOptimize();
//    })
//    if (nElem < 50)
        dumper->build("deoptimized.svg");
    
//    printf("\nSet-get operation on %zu elements ...\n", nElem);
//    TIME_MEASURED({
//        for (size_t i = 0; i < nElem; i++) {
//            int value = 0;
//            list->get(i, &value);
//            list->set(i, value);
//        }
//    })
    
    printf("\nOptimization ...\n");
    TIME_MEASURED({
        list->optimize();
    })
    
//    if (nElem < 50)
        dumper->build("optimized.svg");
    
//    printf("\nSet-get operation on %zu elements ...\n", nElem);
//    TIME_MEASURED({
//        for (size_t i = 0; i < nElem; i++) {
//            int value = 0;
//            list->get(i, &value);
//            list->set(i, value);
//        }
//    })
    
    //    list->optimize();
    //
    //    dumper->build();
    return 0;
}
