//
//  SwiftyList.hpp
//  SwiftyList
//
//  Created by Александр Дремов on 30.10.2020.
//

#ifndef SwiftyList_hpp
#define SwiftyList_hpp

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_INCREASE 16
#define PERFORM_CHECKS {if (this->useChecks){ListOpResult resCheck = this->checkUp(); if (resCheck != LIST_OP_OK) return resCheck;}}
#define DOTPATH "/usr/local/bin/dot"

/**
 * Verbose levels:
 * 0 - No dump at all
 * 1 - Dump only when requested or fail occures
 * 2 - Dump almost all operations
 */

enum ListOpResult {
    LIST_OP_OK,
    LIST_OP_NOMEM,
    LIST_OP_CORRUPTED,
    LIST_OP_OVERFLOW,
    LIST_OP_UNDERFLOW,
    LIST_OP_NOTFOUND
};

template<typename ListElem>
struct SwiftyListNode {
    ListElem value;
    size_t next;
    size_t previous;
};

struct ListStringView {
    char *storage;
    size_t len;
};

struct SwiftyListParams {
    //private:
    short int verbose;
    bool useChecks;
    ListStringView logDir;
    FILE *logFile;

    //public:

    SwiftyListParams(short int verbose, char *logDir, bool useChecks, FILE *logFile) {
        this->verbose = verbose;
        ListStringView logDirString = {};
        if (logDir != NULL)
            logDirString.len = strlen(logDir);
        logDirString.storage = logDir;
        this->logDir = logDirString;
        this->useChecks = useChecks;
        this->logFile = logFile;
    }

    short int getVerbose() {
        return this->verbose;
    }

    bool getChecks() {
        return this->useChecks;
    }

    ListStringView getLogDir() {
        return this->logDir;
    }

    FILE *getLogFile() {
        return this->logFile;
    }
};

template<typename ListElem>
struct SwiftyList {
private:
    SwiftyListNode<ListElem> *storage;
    bool optimized;
    bool useChecks;
    size_t capacity;
    size_t size;
    struct SwiftyListParams *params;

    size_t getFreePos() {
        ListOpResult reallocRes = this->reallocate(true);
        if (reallocRes != LIST_OP_OK)
            return 0;
        return this->size + 1;
    }

    ListOpResult reallocate(bool onlyUp) {
        PERFORM_CHECKS;
        size_t newCapacity = this->capacity;
        if (newCapacity == 0) {
            newCapacity = INITIAL_INCREASE;
        } else {
            if (this->size >= this->capacity) {
                newCapacity = this->capacity * 2;
            }
        }

        if (!onlyUp) {
            if (this->capacity >= this->size * 4) {
                newCapacity = newCapacity / 2;
            }
        }

        if (newCapacity == this->capacity)
            return LIST_OP_OK;

        SwiftyListNode<ListElem> *const newStorage = (SwiftyListNode<ListElem> *) realloc(this->storage,
                                                                                          (newCapacity + 1) *
                                                                                          sizeof(SwiftyListNode<ListElem>));
        if (newStorage == NULL)
            return LIST_OP_NOMEM;
        this->storage = newStorage;
        this->capacity = newCapacity;
        return LIST_OP_OK;
    }

    struct ListGraphDumper {
        // private:
        FILE *file;
        char *filePath;

        SwiftyList<ListElem> *list;

        void dumpNodes() {
            for (size_t i = 0; i <= this->list->getSize(); i++) {
                this->dumpNode(i);
            }
        }

        void dumpNode(size_t node) {
            fprintf(this->file, "node%zu [label=", node);
            fprintf(this->file, "<<table border=\"0\" cellspacing=\"0\"><tr>");
            if (node == 0) {
                fprintf(this->file, "<td colspan=\"3\" bgcolor=\"lightyellow\" port=\"f0h\" border=\"1\">h+t</td>");
            } else {
                fprintf(this->file, "<td colspan=\"3\" bgcolor=\"pink\" port=\"f0h\" border=\"1\">%zu</td>", node);
            }
            int tmp = 0;
            this->list->get(node, &tmp);
            fprintf(this->file, "</tr><tr><td color=\"darkred\" port=\"f0\" border=\"1\">%zu</td>",
                    this->list->storage[node].previous);
            fprintf(this->file, "<td bgcolor=\"darkblue\" port=\"f1\" border=\"1\">");
            fprintf(this->file, "<font color=\"white\">%d</font></td>"
                                "<td color=\"darkgreen\" port=\"f2\" border=\"1\">%zu</td>"
                                "</tr></table>>",
                    this->list->storage[node].value,
                    this->list->storage[node].next);
            fprintf(this->file, "];\n");
        }

        void drawGraphs() {
            for (size_t i = 0; i <= this->list->size; i++) {
                if (i == 0) {
                    fprintf(this->file, "node%zu:f2 -> node%zu:f0 [weight = 100, color=darkgreen, constraint=false]\n",
                            i,
                            this->list->storage[i].next);
                    fprintf(this->file, "node%zu:f0h -> node%zu:f0h [weight = 100, color=darkred, constraint=false]\n",
                            i,
                            this->list->storage[i].previous);
                } else {
                    fprintf(this->file, "node%zu:f2 -> node%zu:f0h [weight = 100, color=darkgreen, constraint=false]\n",
                            i, this->list->storage[i].next);
                    fprintf(this->file, "node%zu:f0 -> node%zu:f1 [weight = 100, color=darkred,  constraint=false]\n",
                            i,
                            this->list->storage[i].previous);
                }
            }
        }

        // public:
        ListGraphDumper(SwiftyList<ListElem> *list, char *tmpFile) {
            this->filePath = tmpFile;
            this->list = list;
        }


        void build(char *imgPath) {
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
            free(compiledCommand);
        }
    };

public:

    SwiftyList(size_t initialSize, short int verbose, char *logDir, FILE *logFile, bool useChecks) :
            optimized(true),
            capacity(initialSize),
            size(0),
            useChecks(useChecks) {
        this->storage = (SwiftyListNode<ListElem> *) calloc(initialSize + 1, sizeof(SwiftyListNode<ListElem>));
        this->params = new SwiftyListParams(verbose, logDir, useChecks, logFile);
    }

    void buildGraph(char* imgName){

    }

    ~SwiftyList() {
        free(this->storage);
    }

    void swapPhysicOnly(size_t firstPos, size_t secondPos) {
        if (secondPos > this->size || firstPos > this->size)
            return;
        if (secondPos == firstPos)
            return;
        firstPos++;
        secondPos++;
        this->optimized = false;

        if (this->storage[firstPos].previous == secondPos) {
            size_t tmpPos = firstPos;
            firstPos = secondPos;
            secondPos = tmpPos;
        }

        const SwiftyListNode<ListElem> tmpFirst = this->storage[firstPos];
        this->storage[firstPos] = this->storage[secondPos];
        this->storage[secondPos] = tmpFirst;

        this->storage[tmpFirst.previous].next = secondPos;
        this->storage[this->storage[firstPos].next].previous = firstPos;

        if (this->storage[firstPos].previous == firstPos || this->storage[firstPos].next == firstPos) {
            this->storage[secondPos].next = firstPos;
            this->storage[firstPos].previous = secondPos;
        } else {
            this->storage[tmpFirst.next].previous = secondPos;
            this->storage[this->storage[firstPos].previous].next = firstPos;
        }
    }

    SwiftyListParams *getParams() {
        return this->params;
    }

    ListOpResult set(size_t pos, const ListElem value) {
        PERFORM_CHECKS;
        if (pos >= this->size)
            return LIST_OP_UNDERFLOW;
        if (this->optimized) {
            this->storage[pos + 1].value = value;
        } else {
            size_t iterator = this->storage[0].next;
            for (size_t i = 0; i < pos; i++) {
                iterator = this->storage[iterator].next;
            }
            this->storage[iterator].value = value;

        }
        return LIST_OP_OK;
    }

    ListOpResult get(size_t pos, ListElem *value) {
        PERFORM_CHECKS;
        if (pos >= this->size)
            return LIST_OP_UNDERFLOW;
        if (this->optimized) {
            *value = this->storage[pos + 1].value;
        } else {
            size_t iterator = 0;
            for (size_t i = 0; i <= pos; i++) {
                iterator = this->storage[iterator].next;
            }
            *value = this->storage[iterator].value;
        }
        return LIST_OP_OK;
    }

    ListOpResult insert(size_t pos, ListElem value) {
        if (pos > this->size)
            return LIST_OP_OVERFLOW;
        if (pos != this->size)
            this->optimized = false;
        size_t posNext = (pos + 1) % (this->size + 1);
        if (!this->optimized) {
            size_t iterator = 0;
            for (size_t i = 0; i < posNext; i++) {
                iterator = this->storage[iterator].next;
            }
            posNext = iterator;
        }
        size_t posNew = this->getFreePos();
        if (posNew == 0)
            return LIST_OP_NOMEM;
        this->storage[posNew].next = posNext;
        this->storage[posNew].previous = this->storage[posNext].previous;
        this->storage[posNew].value = value;

        this->storage[this->storage[posNext].previous].next = posNew;
        this->storage[posNext].previous = posNew;

        this->size++;
        return LIST_OP_OK;
    }

    ListOpResult pop(size_t pos, ListElem *value) {
        PERFORM_CHECKS;
        if (pos >= this->size)
            return LIST_OP_UNDERFLOW;
        if (!this->optimized) {
            size_t iterator = 0;
            for (size_t i = 0; i <= pos; i++) {
                iterator = this->storage[iterator].next;
            }
            pos = iterator;
        } else {
            pos++;
        }
        if (pos != this->size - 1)
            this->optimized = false;

        if (value != nullptr) {
            *value = this->storage[pos].value;
        }

        this->swapPhysicOnly(pos - 1, this->size - 1);
        pos = this->size;

        this->storage[this->storage[pos].previous].next = this->storage[pos].next;
        this->storage[this->storage[pos].next].previous = this->storage[pos].previous;

        this->size--;
        this->reallocate(false);
        return LIST_OP_OK;
    }

    ListOpResult pushFront(const ListElem value) {
        return this->insert(0, value);
    }

    ListOpResult pushBack(const ListElem value) {
        return this->insert(this->size, value);
    }

    ListOpResult popFront(ListElem *value) {
        return this->pop(0, value);
    }

    ListOpResult popBack(ListElem *value) {
        return this->pop(this->size - 1, value);
    }

    ListOpResult search(size_t *pos, const ListElem value) {
        PERFORM_CHECKS;
        if (this->size == 0) {
            return LIST_OP_NOTFOUND;
        }
        *pos = this->storage[0].next;
        while (true) {
            if (this->storage[*pos].value == value) {
                (*pos)--;
                return LIST_OP_OK;
            }

            if (*pos == this->storage[0].previous)
                break;
            *pos = this->storage[*pos].next;
        }
        return LIST_OP_NOTFOUND;
    }

    ListOpResult remove(size_t pos) {
        return this->pop(pos, nullptr);
    }

    ListOpResult resize(size_t nElements) {
        PERFORM_CHECKS;
        size_t sumSizes = this->size;
        if (nElements < sumSizes) {
            nElements = sumSizes;
        }

        nElements++;
        SwiftyListNode<ListElem> *newStorage = (SwiftyListNode<ListElem> *) realloc(this->storage, nElements *
                                                                                                   sizeof(SwiftyListNode<ListElem>));

        if (newStorage == nullptr) return LIST_OP_NOMEM;

        this->storage = newStorage;
        this->capacity = nElements - 1;
        return LIST_OP_OK;
    }

    ListOpResult swap(size_t firstPos, size_t secondPos) {
        if (firstPos >= this->size || secondPos >= this->size)
            return LIST_OP_OVERFLOW;
        size_t first = firstPos;
        size_t second = secondPos;
        if (!this->optimized) {
            size_t iterator = 0;
            for (size_t i = 0; i <= firstPos; i++) {
                iterator = this->storage[iterator].next;
            }
            first = iterator;
        } else {
            first++;
        }

        if (!this->optimized) {
            size_t iterator = 0;
            for (size_t i = 0; i <= secondPos; i++) {
                iterator = this->storage[iterator].next;
            }
            second = iterator;
        } else {
            second++;
        }

        ListElem tmp = this->storage[first].value;
        this->storage[first].value = this->storage[second].value;
        this->storage[second].value = tmp;
        return LIST_OP_OK;
    }

    ListOpResult clear() {
        PERFORM_CHECKS;
        this->size = 0;
        this->storage[0].next = 0;
        this->storage[0].previous = 0;
        this->reallocate(false);
        return LIST_OP_OK;
    }

    ListOpResult optimize() {
        PERFORM_CHECKS;
        for (size_t i = 0; i < this->size; i++) {
            size_t nextPos = this->storage[i].next - 1;
            if (i < nextPos)
                this->swapPhysicOnly(i, nextPos);
        }
        this->optimized = true;
        return LIST_OP_OK;
    }

    ListOpResult deOptimize() {
        PERFORM_CHECKS;
        for (size_t i = 0; i < this->size; i++) {
            int rPos = rand();
            int lPos = rand();
            this->swapPhysicOnly(rPos % this->size, lPos % this->size);
        }
        return LIST_OP_OK;
    }

    ListOpResult checkUp() {
        if (this->size == 0)
            return LIST_OP_OK;

        size_t pos = 0;
        for (size_t i = 0; i <= this->size; i++) {
            SwiftyListNode<ListElem> node = this->storage[pos];
            if ((node.next == pos || node.previous == pos) && this->size > 1)
                return LIST_OP_CORRUPTED;
            pos = node.next;
        }

        if (pos != 0)
            return LIST_OP_CORRUPTED;

        return LIST_OP_OK;
    }

    ListOpResult print() {
        PERFORM_CHECKS;
        size_t pos = this->storage[0].next;
        if (pos == 0 || this->size == 0)
            return LIST_OP_OK;
        while (true) {
            printf("%d ", this->storage[pos].value);
            if (pos == this->storage[0].previous)
                break;
            pos = this->storage[pos].next;
        }
        return LIST_OP_OK;
    }

    ListOpResult shrinkToFit() {
        return this->resize(0);
    }

    size_t getSize() const {
        return this->size;
    }

    size_t getCapacity() const {
        return this->capacity;
    }

    bool isOptimized() {
        return this->optimized;
    }

    bool isEmpty() {
        return this->size == 0;
    }
};


#endif /* SwiftyList_hpp */
