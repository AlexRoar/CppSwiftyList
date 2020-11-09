/**
 * SwiftyList - optimized list with physical and logical interfaces
 *
 * Verbose levels:
 * 0 - No dump at all
 * 1 - Dump only when requested or fail occurs
 * 2 - Dump almost all operations
 *
 * Created by Aleksandr Dremov on 30.10.2020.
 */

#ifndef SwiftyList_hpp
#define SwiftyList_hpp

#include <cstdlib>
#include <cstdio>
#include <cstring>

#define DOTPATH "/usr/local/bin/dot"
#define PERFORM_CHECKS(where) {                             \
if (this->useChecks || this->params->getVerbose() != 0){    \
    ListOpResult resCheck = this->checkUp();                \
    this->opDumper(resCheck, where);                        \
    if (resCheck != LIST_OP_OK) return resCheck;            \
    }                                                       \
}

#define DUMP_STATUS_REASON(status, reason) this->opDumper(status, reason)
const size_t INITIAL_INCREASE = 16;

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

        /**
         * Calls dumpNode() for every node in the list
         */
        void dumpNodes() {
            for (size_t i = 0; i <= this->list->sumSize(); i++) {
                this->dumpNode(i);
            }
            for (size_t i = 0; i <= this->list->sumSize(); i++) {
                int tmp = 0;
                if (this->list->addressValid(i))
                    this->list->get(i, &tmp);
                fprintf(this->file, "nodePhysic%zu [label=<<table border=\"0\" cellspacing=\"0\"><tr>", i);
                if (i == 0){
                    fprintf(this->file, "<td bgcolor=\"lightyellow\" border=\"1\" colspan=\"3\">h+t</td>");
                }else {
                    if (!this->list->storage[i].valid)
                        fprintf(this->file, "<td bgcolor=\"green\" border=\"1\" colspan=\"3\">%zu</td>", i);
                    else
                        fprintf(this->file, "<td bgcolor=\"pink\" border=\"1\" colspan=\"3\">%zu</td>", i);
                }
                fprintf(this->file, "</tr><tr>"
                                    "<td border=\"1\" port=\"f1\">%zu</td><td bgcolor=\"darkblue\" port=\"f0\" border=\"1\"><font color=\"white\">%d</font></td><td port=\"f2\" border=\"1\">%zu</td>"
                                    "</tr></table>>]\n", this->list->storage[i].previous, tmp, this->list->storage[i].next);
                if (i != this->list->sumSize())
                    fprintf(this->file, "nodePhysic%zu -> nodePhysic%zu [style=invis]", i, i + 1);
            }
        }

        /**
         * Dump node in graphviz format
         */
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
            if (this->list->addressValid(node))
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

        /**
         * Define links between nodes
         */
        void drawGraphs() {
            for (size_t i = 0; i <= this->list->sumSize(); i++) {
                if (i == 0) {
                    fprintf(this->file, "node%zu:f2 -> node%zu:f0 [weight = 100, color=darkgreen, constraint=false]\n",
                            i,
                            this->list->storage[i].next);
                    fprintf(this->file, "nodePhysic%zu:f0 -> nodePhysic%zu:f0 [weight = 100, color=darkgreen, constraint=false]\n",
                            i,
                            this->list->storage[i].next);
                    fprintf(this->file, "node%zu:f0h -> node%zu:f0h [weight = 100, color=darkred, constraint=false]\n",
                            i,
                            this->list->storage[i].previous);
                    fprintf(this->file, "nodePhysic%zu:f0 -> nodePhysic%zu:f0 [weight = 100, color=darkred, constraint=false]\n",
                            i,
                            this->list->storage[i].previous);
                } else {
                    fprintf(this->file, "node%zu:f2 -> node%zu:f1 [weight = 100, color=darkgreen]\n",
                            i, this->list->storage[i].next);
                    fprintf(this->file, "nodePhysic%zu:f0 -> nodePhysic%zu:f1 [color=darkgreen, constraint=false]\n",
                            i, this->list->storage[i].next);
                    if (!(!this->list->storage[i].valid && this->list->storage[i].previous == i)) {
                        fprintf(this->file, "node%zu:f0 -> node%zu:f1 [weight = 100, color=darkred]\n",
                                i,
                                this->list->storage[i].previous);
                        fprintf(this->file, "nodePhysic%zu:f0 -> nodePhysic%zu:f2 [color=darkred, constraint=false]\n",
                                i,
                                this->list->storage[i].previous);
                    }
                }
            }
        }

    public:
        ListGraphDumper(SwiftyList<ListElem> *list, char *filePath) : filePath(filePath), list(list), file(nullptr) {}

        /**
         * Generate graph image
         */
        void build(char *imgPath) {
            this->file = fopen(this->filePath, "wb");
            if (this->file == nullptr){
                printf("Failed opening file\n");
                return;
            }
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
        
        ~ListGraphDumper(){}
    };

    struct SwiftyListNode {
        ListElem value;
        size_t   next;
        size_t   previous;
        bool     valid;
    };

    struct SwiftyListParams {
    private:
        short int verbose;
        bool      useChecks;
        FILE      *logFile;

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

    /**
     * Retrieves next possible free pos at all costs.
     * Reallocates container if needed.
     */
    size_t getFreePos(bool mutating = false) {
        if (freeSize != 0) {
            size_t newPos = this->freePtr;
            if (mutating) {
                this->freeSize--;
                this->freePtr = this->storage[newPos].next;
            }
            this->storage[newPos].valid = true;
            return newPos;
        }
        if (this->reallocate() != LIST_OP_OK) {
            DUMP_STATUS_REASON(LIST_OP_NOMEM, "not enough memory");
            return 0;
        }
        this->storage[this->size + 1].valid = true;
        return  this->size + 1;
    }

    /**
     * Reallocates container so that it can hold one more value
     * Reallocation is not performed if some freeSize cells are available.
     */
    ListOpResult reallocate() {
        if (this->freeSize != 0)
            return LIST_OP_OK;
        if (this->size < this->capacity) return LIST_OP_OK;
        size_t newCapacity = this->capacity;
        if (this->size >= this->capacity)
            newCapacity = (this->capacity == 0) ? INITIAL_INCREASE : this->capacity * 2;
        
        if (this->capacity == newCapacity + 2)
            return LIST_OP_OK;

        SwiftyList<ListElem>::SwiftyListNode *newStorage = (SwiftyList<ListElem>::SwiftyListNode *)
                                            realloc(this->storage,(newCapacity + 2)
                                                    * sizeof(SwiftyList<ListElem>::SwiftyListNode));
        if (newStorage == NULL)
            return LIST_OP_NOMEM;
        this->storage = newStorage;
        this->capacity = newCapacity;
        return LIST_OP_OK;
    }

    /**
     * Generates random image name
     */
    char* genRandomImageName(int len) const {
        len += sizeof(".svg");
        char *tmp_s = (char *) calloc((len + 10), sizeof(char));
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

    /**
     * Add released cell to the free poses list
     */
    void addFreePos(size_t pos) {
        this->storage[pos].valid = false;
        this->storage[pos].previous = pos;
        this->storage[pos].next = pos;
        if (this->freeSize == 0) {
            this->freeSize = 1;
            this->freePtr = pos;
        } else {
            this->freeSize++;
            this->storage[pos].next = this->freePtr;
            this->freePtr = pos;
        }
    }

    /**
     * Dumps information about list if needed
     */
    void opDumper(ListOpResult status, const char* where) const {
        if (this->params->getVerbose() == 0)
            return;
        if (this->params->getVerbose() == 1 && status == LIST_OP_OK)
            return;
        char* dumpInfo = (char*)calloc(sizeof("Logging : ") + strlen(where) + 50, sizeof(char));
        sprintf(dumpInfo, "%10s: Logging : \"%s\"",(status == LIST_OP_OK)? "[OK]": "[CAUTION]" , where);

        this->dumpAll((const char*)dumpInfo);

        free(dumpInfo);
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
        this->dumper = new SwiftyList::ListGraphDumper(this, (char*)"tmp.gv");
        this->storage[0].next = 0;
        this->storage[0].previous = 0;
        this->storage[0].valid = false;
    }

    /**
    * Convert logic position to the physic one
    */
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

    /**
     * Insert an element after pos
     * @param pos - physical pos of considered element
     * @param value - value to be inserted
     * @param physPos - physical position of inserted element
     * @return operation result
     */
    ListOpResult insertAfter(size_t pos, ListElem value, size_t* physPos=nullptr) {
        PERFORM_CHECKS("Insert after setting up");
        if (pos > this->sumSize()) {
            DUMP_STATUS_REASON(LIST_OP_OVERFLOW, "insert pos overflow");
            return LIST_OP_OVERFLOW;
        }
        if (!this->addressValid(pos) && pos != 0) {
            DUMP_STATUS_REASON(LIST_OP_SEGFAULT, "insert segmentation fault");
            return LIST_OP_SEGFAULT;
        }
        if (pos != this->storage[0].previous)
            this->optimized = false;

        size_t newPos = this->getFreePos(true);
        if (newPos == 0)
            return LIST_OP_NOMEM;

        if (physPos != nullptr)
            *physPos = newPos; // TODO: consider phy

        this->storage[newPos].value = value;
        this->storage[newPos].previous = pos;
        this->storage[newPos].next = this->storage[pos].next;

        this->storage[this->storage[pos].next].previous = newPos;
        this->storage[pos].next = newPos;

        this->size++;

        PERFORM_CHECKS("Insert after tear down")
        return LIST_OP_OK;
    }

    /**
     * Insert an element before pos
     * @param pos - physical pos of considered element
     * @param value - value to be inserted
     * @param physPos - physical position of inserted element
     * @return operation result
     */
    ListOpResult insertBefore(size_t pos, ListElem value, size_t* physPos=nullptr) {
        if (pos > this->sumSize()) {
            DUMP_STATUS_REASON(LIST_OP_OVERFLOW, "insertBefore pos overflow");
            return LIST_OP_OVERFLOW;
        }
        if (!this->addressValid(pos) && pos != 0) {
            DUMP_STATUS_REASON(LIST_OP_SEGFAULT, "insertBefore segmentation fault");
            return LIST_OP_SEGFAULT;
        }

        pos = this->storage[pos].previous;
        return this->insertAfter(pos, value, physPos);
    }

    /**
     * Insert an element at the first position
     * @param value - value to be inserted
     * @param physPos - physical position of inserted element
     * @return operation result
     */
    ListOpResult pushFront(const ListElem value, size_t* physPos=nullptr) {
        return this->insertAfter(0, value, physPos);
    }

    /**
     * Insert an element at the last position
     * @param value - value to be inserted
     * @param physPos - physical position of inserted element
     * @return operation result
     */
    ListOpResult pushBack(const ListElem value, size_t* physPos=nullptr) {
        return this->insertAfter(this->storage[0].previous, value, physPos);
    }

    /**
     * Set an element at the physical position pos to the new value
     * @param pos - physical pos of considered element
     * @param value - new value
     * @return operation result
     */
    ListOpResult set(size_t pos, const ListElem value) {
        PERFORM_CHECKS("Set setting up");
        if (!this->addressValid(pos)) {
            DUMP_STATUS_REASON(LIST_OP_SEGFAULT, "set segmentation fault");
            return LIST_OP_SEGFAULT;
        }
        this->storage[pos].value = value;
        return LIST_OP_OK;
    }

    /**
     * Get an element at the physical position pos
     * @param pos - physical pos of considered element
     * @param value - retrieved value
     * @return operation result
     */
    ListOpResult get(size_t pos, ListElem* value) {
        if (!this->storage[pos].valid) {
            DUMP_STATUS_REASON(LIST_OP_SEGFAULT, "get segmentation fault");
            return LIST_OP_SEGFAULT;
        }
        if (value == nullptr){
            DUMP_STATUS_REASON(LIST_OP_SEGFAULT, "get nullptr detected");
            return LIST_OP_SEGFAULT;
        }
        *value = this->storage[pos].value;
        return LIST_OP_OK;
    }

    /**
     * Retrieve an element at the physical position pos and remove it
     * @param pos - physical pos of considered element
     * @param value - retrieved value
     * @return operation result
     */
    ListOpResult pop(size_t pos, ListElem *value=nullptr) {
        PERFORM_CHECKS("Pop setting up");
        if (this->size == 0) {
            DUMP_STATUS_REASON(LIST_OP_UNDERFLOW, "pop pos underflow");
            return LIST_OP_UNDERFLOW;
        }
        if (!this->addressValid(pos)) {
            this->opDumper(LIST_OP_SEGFAULT, "pop invalid address");
            return LIST_OP_SEGFAULT;
        }

        if (pos != this->storage[0].previous)
            this->optimized = false;

        if (value != nullptr)
            *value = this->storage[pos].value;

        this->storage[this->storage[pos].next].previous = this->storage[pos].previous;
        this->storage[this->storage[pos].previous].next = this->storage[pos].next;

        this->addFreePos(pos);
        this->size--;

        PERFORM_CHECKS("Pop tear down");
        return LIST_OP_OK;
    }

    /**
     * Retrieve an element at the beginning and remove it
     * @param value - retrieved value
     * @return operation result
     */
    ListOpResult popFront(ListElem *value) {
        return this->pop(this->storage[0].next, value);
    }

    /**
     * Retrieve an element at the end and remove it
     * @param value - retrieved value
     * @return operation result
     */
    ListOpResult popBack(ListElem *value) {
        return this->pop(this->storage[0].previous, value);
    }

    /**
     * Remove an element at the physical position pos
     * @param pos - physical pos of considered element
     * @return operation result
     */
    ListOpResult remove(size_t pos) {
        return this->pop(pos, nullptr);
    }

    /**
     * Swap two elements at the physical positions
     * @param firstPos - physical pos of the first element
     * @param secondPos - physical pos of the second element
     * @return operation result
     */
    ListOpResult swap(size_t firstPos, size_t secondPos) {
        PERFORM_CHECKS("Swap setting up");
        if (firstPos == secondPos)
            return LIST_OP_OK;
        if (!this->addressValid(firstPos) || !this->addressValid(secondPos)) {
            DUMP_STATUS_REASON(LIST_OP_SEGFAULT, "swap segmentation fault");
            return LIST_OP_SEGFAULT;
        }

        ListElem tmp = this->storage[firstPos].value;
        this->storage[firstPos].value = this->storage[secondPos].value;
        this->storage[secondPos].value = tmp;
        PERFORM_CHECKS("Swap tear down");
        return LIST_OP_OK;
    }

    /**
     * Clears the list
     * @return operation result
     */
    ListOpResult clear() {
        PERFORM_CHECKS("Clear setting up");
        this->size = 0;
        this->storage[0].next = 0;
        this->storage[0].previous = 0;
        this->freeSize = 0;
        this->freePtr  = 0;
        this->reallocate();
        PERFORM_CHECKS("Clear tear down");
        return LIST_OP_OK;
    }

    /**
     * Optimizes the list so that logical access is effective and
     * physical positions are aligned in ascending order in the storage
     * @return operation result
     */
    ListOpResult optimize() {
        PERFORM_CHECKS("Optimize setting up");
        SwiftyList<ListElem>::SwiftyListNode *newStorage = (SwiftyList<ListElem>::SwiftyListNode*)(calloc(this->size + 2, sizeof(SwiftyList<ListElem>::SwiftyListNode)));
        if (newStorage == nullptr) {
            DUMP_STATUS_REASON(LIST_OP_NOMEM, "optimize no memory");
            return LIST_OP_NOMEM;
        }
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
        free(this->storage);
        this->storage = newStorage;
        this->capacity = this->size;
        PERFORM_CHECKS("Optimize tear down");
        return LIST_OP_OK;
    }

    /**
     * Moves iterator to the next physical position
     * @param pos
     * @return
     */
    ListOpResult nextIterator(size_t* pos) {
        if (!this->addressValid(*pos))
            return LIST_OP_SEGFAULT;
        *pos =  this->storage[*pos].next;
    }

    /**
    * Moves iterator to the previous physical position
    * @param pos
    * @return
    */
    ListOpResult prevIterator(size_t* pos) {
        if (!this->addressValid(*pos))
            return LIST_OP_SEGFAULT;
        *pos =  this->storage[*pos].next;
    }

    /**
     * Check up list's integrity
     * @return operation result
     */
    ListOpResult checkUp() const {
        if (this->size == 0)
            return LIST_OP_OK;
        if (this->size > this->capacity)
            return LIST_OP_CORRUPTED;

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

    /**
     * Deoptimizes list
     * @return operation result
     */
    ListOpResult deOptimize() {
        PERFORM_CHECKS("Deoptimize setting up");
        this->optimized = false;
        return LIST_OP_OK;
    }

    /**
     * Resizes list to desired number of elements. If elements number is lower than current one,
     * shrinks to fit.
     * @return operation result
     */
    ListOpResult resize(size_t elemNumbers) {
        PERFORM_CHECKS("Resize setting up");
        if (elemNumbers < this->sumSize())
            elemNumbers = this->sumSize();
        elemNumbers++;
        ListElem *newStorage = calloc(elemNumbers, sizeof(ListElem));
        if (newStorage == nullptr)
            return LIST_OP_NOMEM;
        this->storage = newStorage;
        this->capacity = elemNumbers - 1;
        PERFORM_CHECKS("Resize tear down");
        return LIST_OP_OK;
    }

    /**
     * Resizes list to the minimum available space
     * @return operation result
     */
    ListOpResult shrinkToFit() {
        return this->resize(0);
    }

    /**
     * Search an element in the list. Retrieves the logical position
     * @param pos - logical pos of considered element
     * @param value - searched value
     * @return operation result
     */
    ListOpResult searchLinear(size_t *pos, const ListElem value) const{
        PERFORM_CHECKS("Search setting up");
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
        PERFORM_CHECKS("Search tear down");
        return LIST_OP_NOTFOUND;
    }

    /**
     * Dump all information as new section
     */
    void dumpAll(const char* sectionName) const{
        if (this->params->getLogFile() != NULL) {
            this->setNewSection(sectionName);
            this->dumpData();
            this->dumpImage();
        }
    }

    /**
     * Dump only basic list information
     */
    void dumpData() const{
        FILE* logFile = this->params->getLogFile();
        fprintf(logFile, "\n<pre><code>\n");
        if (logFile == NULL) return;

        fprintf(logFile, "SwiftyList [%p] {\n", this);
        fprintf(logFile, "\tstorage    :  [%p]\n",  this->storage);
        fprintf(logFile, "\tcapacity   :  %zu\n", this->capacity);
        fprintf(logFile, "\tsize       :  %zu\n", this->size);
        fprintf(logFile, "\tfreeSize   :  %zu\n", this->freeSize);
        fprintf(logFile, "\tsumSize    :  %zu\n", this->sumSize());
        fprintf(logFile, "\toptimized  :  %d\n", this->optimized);
        fprintf(logFile, "\tuseChecks  :  %d\n", this->useChecks);
        fprintf(logFile, "\tvalid      :  %s\n", (this->checkUp() == LIST_OP_OK)? "YES": "NO");
        fprintf(logFile, "\tfictive {\n");
        fprintf(logFile, "\t      head :  %zu\n", this->storage[0].next);
        fprintf(logFile, "\t      tail :  %zu\n", this->storage[0].previous);
        fprintf(logFile, "\t}\n");
        fprintf(logFile, "}\n");
        fprintf(logFile, "\n</code></pre>\n");
    }

    /**
     * Sets new logging section
     */
    void setNewSection(const char *section) const{
        if (this->params->getLogFile() != NULL)
            fprintf(this->params->getLogFile(), "<h2>%s</h2>\n", section);
    }

    /**
     * Creates image and adds the reference to it into the log file
     */
    void dumpImage() const{
        char *name = this->genRandomImageName(20);
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

    SwiftyListParams *getParams() const{
        return this->params;
    }

    size_t sumSize() const{
        return this->size + this->freeSize;
    }

    bool addressValid(size_t pos) const {
        return this->storage[pos].valid;
    }
    
    void destructList(){
        free(this->storage);
    }
    
    ~SwiftyList() {}
};


#endif /* SwiftyList_hpp */
