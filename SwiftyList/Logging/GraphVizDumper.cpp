//
//  GraphVizDumper.cpp
//  SwiftyList
//
//  Created by Александр Дремов on 31.10.2020.
//

#include "SwiftyList.hpp"
#include <stdio.h>
#include <string.h>

ListGraphDumper::ListGraphDumper(SwiftyList *list, char *tmpFile) {
    this->filePath = tmpFile;
    this->list = list;
}

void ListGraphDumper::dumpNodes() {
    for (size_t i = 0; i <= this->list->size; i++) {
        this->dumpNode(i);
    }
}

void ListGraphDumper::dumpNode(size_t node) {
    fprintf(this->file, "node%zu [label=", node);
    fprintf(this->file, "<<table border=\"0\" cellspacing=\"0\"><tr>");
    if (node == 0) {
        fprintf(this->file, "<td colspan=\"3\" bgcolor=\"lightyellow\" port=\"f0h\" border=\"1\">h+t</td>");
    } else {
        fprintf(this->file, "<td colspan=\"3\" bgcolor=\"pink\" port=\"f0h\" border=\"1\">%zu</td>", node);
    }
    fprintf(this->file, "</tr><tr><td color=\"darkred\" port=\"f0\" border=\"2\">%zu</td>", this->list->storage[node].previous);
    fprintf(this->file, "<td bgcolor=\"darkblue\" port=\"f1\" border=\"1\">");
    fprintf(this->file, "<font color=\"white\">%d</font></td>"
                        "<td color=\"darkgreen\" port=\"f2\" border=\"2\">%zu</td>"
                        "</tr></table>>",
            this->list->storage[node].value,
            this->list->storage[node].next);
    fprintf(this->file, "];\n");
}

void ListGraphDumper::drawGraphs() {
    for (size_t i = 0; i <= this->list->size; i++) {
        if (i == 0) {
            fprintf(this->file, "node%zu:f2 -> node%zu:f0 [weight = 100, color=darkgreen, constraint=false]\n",
                    i,
                    this->list->storage[i].next);
            fprintf(this->file, "node%zu:f0h -> node%zu:f0h [weight = 100, color=darkred, constraint=false]\n",
                    i,
                    this->list->storage[i].previous);
        } else {
            fprintf(this->file, "node%zu:f0h -> node%zu:f1 [weight = 100, color=darkgreen, constraint=false]\n",
                    i, this->list->storage[i].next);
            fprintf(this->file, "node%zu:f0 -> node%zu:f0h [weight = 100, color=darkred,  constraint=false]\n", i,
                    this->list->storage[i].previous);
        }
    }
}

void ListGraphDumper::build(char *imgPath) {
    this->file = fopen(this->filePath, "wb");
    fprintf(this->file, "digraph structs {\nnode [shape=none];\nrank=same;\nrankdir=\"LR\";\n");
    this->dumpNodes();
    this->drawGraphs();
    fprintf(this->file, "\n}\n");
    fflush(this->file);
    const char *command = DOTPATH " -Tsvg %s -o %s";
    char *compiledCommand = (char *) calloc(strlen(command) + strlen(imgPath) + strlen(this->filePath) + 3, 1);
    sprintf(compiledCommand, command, this->filePath, imgPath);
    system(compiledCommand);
    fclose(this->file);
}
