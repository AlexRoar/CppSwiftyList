//
//  GraphVizDumper.hpp
//  SwiftyList
//
//  Created by Александр Дремов on 31.10.2020.
//

#ifndef GraphVizDumper_hpp
#define GraphVizDumper_hpp
#include "SwiftyList.hpp"
#include <stdio.h>

struct ListGraphDumper {
// private:
    FILE* file;
    SwiftyList* list;
    char* imgPath;
    char* filePath;
    void dumpNodes();
    void dumpNode(size_t node);
    void drawGraphs();
// public:
    ListGraphDumper(SwiftyList* list, char* tmpFile, char* imgPath);
    void build();
};

#endif /* GraphVizDumper_hpp */