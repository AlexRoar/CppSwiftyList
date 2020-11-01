//
//  GraphVizDumper.cpp
//  SwiftyList
//
//  Created by Александр Дремов on 31.10.2020.
//

#include "GraphVizDumper.hpp"
#include <stdio.h>
#include <string.h>

ListGraphDumper::ListGraphDumper(SwiftyList* list, char* tmpFile, char* imgPath) {
    this->file = fopen(tmpFile, "wb");
    this->filePath = tmpFile;
    this->list = list;
    this->imgPath = imgPath;
}

void ListGraphDumper::dumpNodes() {
    for(size_t i = 1; i <= this->list->sumSize(); i++) {
        this->dumpNode(i);
    }
}
void ListGraphDumper::dumpNode(size_t node) {
    fprintf(this->file, "node%zu [label=", node);
    
    if (this->list->storage[node].previous != 0){
        fprintf(this->file, "<<table border=\"0\" cellspacing=\"0\"><tr>");
        if (node == this->list->head){
            fprintf(this->file, "<td colspan=\"3\" bgcolor=\"pink\" port=\"f0h\" border=\"1\">h(%zu)</td>", node);
        } else if (node == this->list->tail){
            fprintf(this->file, "<td colspan=\"3\" bgcolor=\"pink\" port=\"f0h\" border=\"1\">t(%zu)</td>", node);
        } else {
            fprintf(this->file, "<td colspan=\"3\" bgcolor=\"pink\" port=\"f0h\" border=\"1\">%zu</td>", node);
        }
        fprintf(this->file, "</tr><tr><td port=\"f0\" border=\"1\">%zu</td>"
                , this->list->storage[node].previous);
        fprintf(this->file, "<td bgcolor=\"darkblue\" port=\"f1\" border=\"1\">");
        fprintf(this->file, "<font color=\"white\">%d</font></td>"
                "<td port=\"f2\" border=\"1\">%zu</td>"
                "</tr></table>>",
                this->list->storage[node].value,
                this->list->storage[node].next);
    } else {
        fprintf(this->file, "<<table border=\"0\" cellspacing=\"0\"><tr>");
        if (node == this->list->head){
            fprintf(this->file, "<td colspan=\"3\" bgcolor=\"lightyellow\" port=\"f0h\" border=\"1\">h(%zu)</td>", node);
        } else if (node == this->list->tail){
            fprintf(this->file, "<td colspan=\"3\" bgcolor=\"lightyellow\" port=\"f0h\" border=\"1\">t(%zu)</td>", node);
        } else {
            fprintf(this->file, "<td colspan=\"3\" bgcolor=\"lightyellow\" port=\"f0h\" border=\"1\">%zu</td>", node);
        }
        fprintf(this->file, "</tr><tr><td port=\"f0\" border=\"1\">%zu</td>"
                , this->list->storage[node].previous);
        fprintf(this->file, "<td bgcolor=\"darkgreen\" port=\"f1\" border=\"1\">");
        fprintf(this->file, "<font color=\"white\">%d</font></td>"
                "<td port=\"f2\" border=\"1\">%zu</td>"
                "</tr></table>>",
                this->list->storage[node].value,
                this->list->storage[node].next);
    }
    fprintf(this->file, "];\n");
}
void ListGraphDumper::drawGraphs() {
    for(size_t i = 1; i <= this->list->sumSize(); i++) {
        if (this->list->storage[i].next <= this->list->sumSize() &&
            this->list->storage[i].next != 0)
            fprintf(this->file, "node%zu:f2 -> node%zu:f1 [weight = 100, color=darkgreen]\n", i, this->list->storage[i].next);
        if (this->list->storage[i].previous <= this->list->sumSize() &&
            this->list->storage[i].previous != 0)
            fprintf(this->file, "node%zu:f0 -> node%zu:f1 [weight = 100, color=darkred]\n", i, this->list->storage[i].previous);
    }
}
void ListGraphDumper::build() {
    fprintf(this->file, "digraph structs {\nnode [shape=none];\nrank=same;\nrankdir=\"LR\";\n");
    this->dumpNodes();
    this->drawGraphs();
    fprintf(this->file, "\n}\n");
    fflush(this->file);
    const char *command = DOTPATH " -Tpng %s -o %s";
    char* compiledCommand = (char*)calloc(strlen(command) + strlen(this->imgPath) + strlen(this->filePath) + 3, 1);
    sprintf(compiledCommand, command, this->filePath, this->imgPath);
    system(compiledCommand);
    fclose(this->file);
}
