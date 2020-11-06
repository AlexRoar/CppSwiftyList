//
//  SwiftyList.hpp
//  SwiftyList
//
//  Created by Александр Дремов on 30.10.2020.
//

#ifndef SwiftyList_hpp
#define SwiftyList_hpp

#include <cstdlib>
#include <cstdio>
#include <cstring>

const size_t INITIAL_INCREASE = 16;
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
    LIST_OP_NOTFOUND,
    LIST_OP_SEGFAULT
};

template<typename ListElem>
struct SwiftyList {
private:
    struct ListGraphDumper;
    struct SwiftyListNode;
    struct SwiftyListParams;

    SwiftyList<ListElem>::ListGraphDumper *dumper;
    SwiftyList<ListElem>::SwiftyListNode *storage;
    SwiftyList<ListElem>::SwiftyListParams *params;

    bool optimized;
    bool useChecks;

    size_t capacity;
    size_t size;

    size_t freePtr;
    size_t freeSize;

    struct ListGraphDumper {
    private:
        FILE *file;
        char *filePath;
        SwiftyList<ListElem> *list;

        void dumpNodes() {
            for (size_t i = 0; i <= this->list->sumSize(); i++) {
                this->dumpNode(i);
            }
        }

        void dumpNode(size_t node) {
            fprintf(this->file, "node%zu [label=", node);
            fprintf(this->file, "<<table border=\"0\" cellspacing=\"0\"><tr>");
            if (node == 0) {
                fprintf(this->file, "<td colspan=\"3\" bgcolor=\"lightyellow\" port=\"f0h\" border=\"1\">h+t</td>");
            } else {
                if ( this->list->storage[node].valid){
                    fprintf(this->file, "<td colspan=\"3\" bgcolor=\"pink\" port=\"f0h\" border=\"1\">%zu</td>", node);
                } else {
                    fprintf(this->file, "<td colspan=\"3\" bgcolor=\"green\" port=\"f0h\" border=\"1\">%zu</td>", node);
                }
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
                    fprintf(this->file, "node%zu:f2 -> node%zu:f1 [weight = 100, color=darkgreen]\n",
                            i, this->list->storage[i].next);
                    fprintf(this->file, "node%zu:f0 -> node%zu:f1 [weight = 100, color=darkred]\n",
                            i,
                            this->list->storage[i].previous);
                }
            }
        }

    public:
        ListGraphDumper(SwiftyList<ListElem> *list, char *filePath) : filePath(filePath), list(list), file(nullptr) {}

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

    struct SwiftyListNode {
        ListElem value;
        size_t next;
        size_t previous;
        bool   valid;
    };

    struct SwiftyListParams {
    private:
        short int verbose;
        bool useChecks;
        FILE *logFile;

    public:
        SwiftyListParams(short verbose, bool useChecks, FILE *logFile) : verbose(verbose), useChecks(useChecks),
                                                                         logFile(logFile) {}
        short int getVerbose() const {
            return this->verbose;
        }
        bool getChecks() const {
            return this->useChecks;
        }
        FILE *getLogFile() const {
            return this->logFile;
        }
    };

    size_t getFreePos(bool mutating = false) {
        if (freeSize != 0) {
            size_t newPos = this->freePtr;
            if (mutating) {
                this->freeSize--;
                this->freePtr = this->storage[newPos].next;
            }
            this->storage[this->freePtr].valid = false;
            return newPos;
        }
        if (this->reallocate() != LIST_OP_OK)
            return 0;
        this->storage[ this->size + 1].valid = true;
        return  this->size + 1;
    }

    ListOpResult reallocate() {
        if (this->freeSize != 0)
            return LIST_OP_OK;
        if (this->size < this->capacity) return LIST_OP_OK;
        size_t newCapacity = this->capacity;
        if (this->size >= this->capacity)
            newCapacity = (this->capacity == 0) ? INITIAL_INCREASE : this->capacity * 2;

        if (this->capacity == newCapacity)
            return LIST_OP_OK;

        SwiftyListNode *const newStorage = (SwiftyListNode *) realloc(this->storage,
                                                                                          (newCapacity + 1) *
                                                                                          sizeof(SwiftyListNode));
        if (newStorage == NULL)
            return LIST_OP_NOMEM;
        this->storage = newStorage;
        this->capacity = newCapacity;
        return LIST_OP_OK;
    }


    size_t logicToPhysic(size_t pos) const {
        if (this->optimized) {
            return pos + 1;
        } else {
            size_t iterator = 0;
            for (size_t i = 0; i <= pos; i++) {
                iterator = this->storage[iterator].next;
            }
            return iterator;
        }
    }

    char *genRandomStringName(int len) {
        len += sizeof(".svg");
        char *tmp_s = (char *) malloc((len + 2) * sizeof(char));
        static const char alphanum[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";

        int i = 0;
        for (; i < len - sizeof(".svg"); ++i)
            tmp_s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
        strcpy(tmp_s + i, ".svg");
        return tmp_s;
    }

    bool inline addressValid(size_t pos) const{
        return this->storage[pos].valid;
    }

    void addFreePos(size_t pos) {
        this->storage[pos].valid = false;
        if (this->freeSize == 0) {
            this->freeSize = 1;
            this->freePtr = pos;
        } else {
            this->freeSize++;
            this->storage[pos].next = this->freePtr;
            this->freePtr = pos;
        }
    }
public:
    SwiftyList(size_t initialSize, short int verbose, FILE *logFile, bool useChecks) :
            optimized(true),
            capacity(initialSize),
            size(0),
            useChecks(useChecks),
            freeSize(0),
            freePtr(0) {
        this->storage = (SwiftyListNode *) calloc(initialSize + 1, sizeof(SwiftyListNode));
        this->params = new SwiftyListParams(verbose, useChecks, logFile);
        this->dumper = new SwiftyList::ListGraphDumper(this, "tmp.gv");
        this->storage[0].next = 0;
        this->storage[0].previous = 0;
        this->storage[0].valid = false;
    }

    ListOpResult insertAfter(size_t pos, ListElem value, size_t* physPos=nullptr) {
        if (pos > this->sumSize())
            return LIST_OP_OVERFLOW;
        if (!this->addressValid(pos) && pos != 0)
            return LIST_OP_SEGFAULT;

        if (pos != this->storage[0].previous)
            this->optimized = false;

        size_t newPos = this->getFreePos(true);
        if (newPos == 0)
            return LIST_OP_NOMEM;

        if (physPos != nullptr)
            *physPos = newPos;

        this->storage[newPos].value = value;
        this->storage[newPos].previous = pos;
        this->storage[newPos].next = this->storage[pos].next;

        this->storage[this->storage[pos].next].previous = newPos;
        this->storage[pos].next = newPos;

        this->size++;

        return LIST_OP_OK;
    }

    ListOpResult insertAfterLogic(size_t pos, ListElem value, size_t* physPos=nullptr) {
        if (pos > this->size)
            return LIST_OP_OVERFLOW;
        return this->insertAfter(this->logicToPhysic(pos), value, physPos);
    }

    ListOpResult insertBefore(size_t pos, ListElem value, size_t* physPos=nullptr) {
        if (pos > this->sumSize())
            return LIST_OP_OVERFLOW;
        if (!this->addressValid(pos) && pos != 0)
            return LIST_OP_SEGFAULT;

        pos = this->storage[pos].previous;
        return this->insertAfter(pos, value, physPos);
    }

    ListOpResult insertBeforeLogic(size_t pos, ListElem value, size_t* physPos=nullptr) {
        if (pos > this->size)
            return LIST_OP_OVERFLOW;
        return this->insertBefore(this->logicToPhysic(pos), value, physPos);
    }

    ListOpResult pushFront(const ListElem value, size_t* physPos=nullptr) {
        return this->insertAfter(0, value, physPos);
    }

    ListOpResult pushBack(const ListElem value, size_t* physPos=nullptr) {
        return this->insertAfter(this->storage[0].previous, value, physPos);
    }

    ListOpResult set(size_t pos, const ListElem value) {
        PERFORM_CHECKS;
        if (!this->addressValid(pos))
            return LIST_OP_SEGFAULT;
        this->storage[pos].value = value;
        return LIST_OP_OK;
    }

    ListOpResult setLogic(size_t pos, const ListElem value) {
        if (pos > this->size)
            return LIST_OP_OVERFLOW;
        return this->set(this->logicToPhysic(pos), value);
    }

    ListOpResult get(size_t pos, ListElem* value) {
        PERFORM_CHECKS;
        if (!this->addressValid(pos))
            return LIST_OP_SEGFAULT;
        *value = this->storage[pos].value;
        return LIST_OP_OK;
    }

    ListOpResult getLogic(size_t pos, ListElem* value=nullptr) {
        if (pos > this->size)
            return LIST_OP_OVERFLOW;
        return this->get(this->logicToPhysic(pos), value);
    }

    ListOpResult pop(size_t pos, ListElem *value=nullptr) {
        PERFORM_CHECKS;
        if (this->size == 0)
            return LIST_OP_UNDERFLOW;
        if (!this->addressValid(pos))
            return LIST_OP_SEGFAULT;

        if (pos != this->storage[0].previous)
            this->optimized = false;

        if (value != nullptr)
            *value = this->storage[pos].value;

        this->storage[this->storage[pos].next].previous = this->storage[pos].previous;
        this->storage[this->storage[pos].previous].next = this->storage[pos].next;

        this->addFreePos(pos);
        this->size--;

        return LIST_OP_OK;
    }

    ListOpResult popFront(ListElem *value) {
        return this->pop(this->storage[0].next, value);
    }

    ListOpResult popBack(ListElem *value) {
        return this->pop(this->storage[0].previous, value);
    }

    ListOpResult popLogic(size_t pos, ListElem *value) {
        return this->pop(this->logicToPhysic(pos), value);
    }

    ListOpResult remove(size_t pos) {
        return this->pop(pos, nullptr);
    }

    ListOpResult removeLogic(size_t pos) {
        if (pos > this->size)
            return LIST_OP_OVERFLOW;
        return this->pop(this->logicToPhysic(pos), nullptr);
    }

    ListOpResult swap(size_t firstPos, size_t secondPos) {
        if (firstPos == secondPos)
            return LIST_OP_OK;
        if (!this->addressValid(firstPos) || !this->addressValid(secondPos))
            return LIST_OP_SEGFAULT;

        ListElem tmp = this->storage[firstPos].value;
        this->storage[firstPos].value = this->storage[secondPos].value;
        this->storage[secondPos].value = tmp;
        return LIST_OP_OK;
    }

    ListOpResult swapLogic(size_t firstPos, size_t secondPos) {
        if (firstPos > this->size || secondPos > this->size)
            return LIST_OP_OVERFLOW;
        return this->swap(this->logicToPhysic(firstPos), this->logicToPhysic(secondPos));
    }

    ListOpResult clear() {
        PERFORM_CHECKS;
        this->size = 0;
        this->storage[0].next = 0;
        this->storage[0].previous = 0;
        this->freeSize = 0;
        this->freePtr  = 0;
        this->reallocate();
        return LIST_OP_OK;
    }

    ListOpResult optimize() {
        PERFORM_CHECKS;
        SwiftyList<ListElem>::SwiftyListNode *newStorage = (SwiftyList<ListElem>::SwiftyListNode*)(calloc(this->size + 1, sizeof(SwiftyList<ListElem>::SwiftyListNode)));
        if (newStorage == nullptr)
            return LIST_OP_NOMEM;
        newStorage[0] = this->storage[0];
        size_t iterator = this->storage[0].next;
        for (size_t i = 0; i < this->size; i++) {
            newStorage[i + 1] = this->storage[iterator];
            iterator = this->storage[iterator].next;
            newStorage[i + 1].previous = i;
            newStorage[i].next = i + 1;
            newStorage[i].valid = true;
            if (i + 1 == this->size) {
                newStorage[i + 1].next = 0;
            }
        }
        this->optimized = true;
        this->freePtr = 0;
        this->freeSize = 0;
        this->storage = newStorage;
        return LIST_OP_OK;
    }

    ListOpResult checkUp() const {
        if (this->size == 0)
            return LIST_OP_OK;

        size_t pos = 0;
        for (size_t i = 0; i <= this->size; i++) {
            SwiftyListNode node = this->storage[pos];
            if ((node.next == pos || node.previous == pos) && this->size > 1)
                return LIST_OP_CORRUPTED;
            pos = node.next;
        }

        if (pos != 0)
            return LIST_OP_CORRUPTED;

        return LIST_OP_OK;
    }

    ListOpResult deOptimize() {
        this->optimized = false;
        return LIST_OP_OK;
    }

    ListOpResult search(size_t *pos, const ListElem value) const{
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

    void dumpAll(const char* sectionName) const{
        this->setNewSection(sectionName);
        this->dumpData();
        this->dumpImage();
    }

    void dumpData() const{
        FILE* logFile = this->params->getLogFile();
        fprintf(logFile, "\n<pre><code>\n");
        if (logFile == NULL) return;

        fprintf(logFile, "SwiftyList [%p] {\n", this);
        fprintf(logFile, "\tcapacity :  %zu\n", this->capacity);
        fprintf(logFile, "\tsize     :  %zu\n", this->size);
        fprintf(logFile, "\thead     :  %zu\n", this->storage[0].next);
        fprintf(logFile, "\ttail     :  %zu\n", this->storage[0].previous);
        fprintf(logFile, "\toptimized:  %d\n", this->optimized);
        fprintf(logFile, "\tuseChecks:  %d\n", this->useChecks);
        fprintf(logFile, "\tvalid    :  %s\n", (this->checkUp() == LIST_OP_OK)? "YES": "NO");
        fprintf(logFile, "}\n");

        fprintf(logFile, "\n</code></pre>\n");
    }

    void setNewSection(const char *section) const{
        if (this->params->getLogFile() != NULL)
            fprintf(this->params->getLogFile(), "<h2>%s</h2>\n", section);
    }

    void dumpImage() const{
        char *name = this->genRandomStringName(20);
        this->dumper->build(name);
        if (this->params->getLogFile() != NULL) fprintf(this->params->getLogFile(), "<img src=\"%s\">\n", name);
        free(name);
    }

    size_t getSize() const {
        return this->size;
    }

    size_t getCapacity() const {
        return this->capacity;
    }

    bool isOptimized() const{
        return this->optimized;
    }

    bool isEmpty() const{
        return this->size == 0;
    }

    ~SwiftyList() {
        free(this->storage);
    }

    SwiftyListParams *getParams() const{
        return this->params;
    }

    size_t sumSize() const{
        return this->size + this->freeSize;
    }
};


#endif /* SwiftyList_hpp */
