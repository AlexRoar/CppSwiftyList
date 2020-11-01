#include <stdio.h>
#include "GraphVizDumper.hpp"
#include "SwiftyList.hpp"

int main() {
    SwiftyList* list = new SwiftyList(0, 2, NULL, NULL, false);
    
    list->pushBack(0);
    list->pushBack(1);
    list->pushBack(2);
    list->pushBack(3);
    list->pushBack(4);
//    list->swapPhysicOnly(3, 4);
//    list->swapPhysicOnly(2, 3);
    list->swapPhysicOnly(0, 1);
//    list->swapPhysicOnly(2, 4);
//    list->swapPhysicOnly(0, 3);
//    list->swapPhysicOnly(0, 3);
//    list->swapPhysicOnly(2, 4);
    
//    list->print(); printf("\n");
    //    list->swapPhysicOnly(0, 1);
    //    list->print(); printf("\n");
    //
    
    //    list->print(); printf("\n");
    //    list->remove(1);
    //
//    list->optimize();
    ListGraphDumper* dumper = new ListGraphDumper(list, (char*)"grapfTst.gv", (char*)"gr.png");
    dumper->build();
    return 0;
}
