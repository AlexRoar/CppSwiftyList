//
//  SwiftyList.cpp
//  SwiftyList
//
//  Created by Александр Дремов on 30.10.2020.
//

#include "SwiftyList.hpp"
#include <string.h>

#define INITIAL_INCREASE 16

#define PERFORM_CHECKS {if (this->useChecks){ListOpResult resCheck = this->checkUp(); if (resCheck != LIST_OP_OK) return resCheck;}}

// back  == tail
// front == head

SwiftyList::SwiftyList(size_t initialSize, short int verbose, char *logDir, FILE *logFile, bool useChecks) :
optimized(true),
capacity(initialSize),
size(0),
useChecks(useChecks){
    this->storage = (SwiftyListNode *) calloc(initialSize + 1, sizeof(SwiftyListNode));
    this->params = new SwiftyListParams(verbose, logDir, useChecks, logFile);
}

size_t SwiftyList::getSize() const {
    return this->size;
}


size_t SwiftyList::getCapacity() const {
    return this->capacity;
}


ListOpResult SwiftyList::clear() {
    PERFORM_CHECKS;
    this->size = 0;
    this->storage[0].next = 0;
    this->storage[0].previous = 0;
    this->reallocate(false);
    return LIST_OP_OK;
}

ListOpResult SwiftyList::insert(size_t pos, ListElem value) {
    if (pos > this->size)
        return LIST_OP_OVERFLOW;
    if (pos != this->size)
        this->optimized = false;
    size_t posNext = (pos + 1) % (this->size + 1);
    if (!this->optimized){
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

ListOpResult SwiftyList::resize(size_t nElements) {
    PERFORM_CHECKS;
    size_t sumSizes = this->size;
    if (nElements < sumSizes) {
        nElements = sumSizes;
    }

    nElements++;
    SwiftyListNode *newStorage = (SwiftyListNode *) realloc(this->storage, nElements * sizeof(SwiftyListNode));
    
    if (newStorage == nullptr) return LIST_OP_NOMEM;
    
    this->storage = newStorage;
    this->capacity = nElements - 1;
    return LIST_OP_OK;
}

ListOpResult SwiftyList::pushBack(const ListElem value) {
    return this->insert(this->size, value);
}

ListOpResult SwiftyList::pushFront(const ListElem value) {
    return this->insert(0, value);
}

size_t SwiftyList::getFreePos() {
    ListOpResult reallocRes = this->reallocate(true);
    if (reallocRes != LIST_OP_OK)
        return 0;
    return  this->size + 1;
}

ListOpResult SwiftyList::get(size_t pos, ListElem *value) {
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

ListOpResult SwiftyList::set(size_t pos, const ListElem value) {
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

ListOpResult SwiftyList::reallocate(bool onlyUp) {
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
    
    SwiftyListNode *const newStorage = (SwiftyListNode *) realloc(this->storage, (newCapacity + 1) * sizeof(SwiftyListNode));
    if (newStorage == NULL)
        return LIST_OP_NOMEM;
    this->storage = newStorage;
    this->capacity = newCapacity;
    return LIST_OP_OK;
}

ListOpResult SwiftyList::print() {
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

ListOpResult SwiftyList::search(size_t *pos, const ListElem value) {
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

SwiftyListParams::SwiftyListParams(short int verbose, char *logDir, bool useChecks, FILE *logFile) {
    this->verbose = verbose;
    ListStringView logDirString = {};
    if (logDir != NULL)
        logDirString.len = strlen(logDir);
    logDirString.storage = logDir;
    this->logDir = logDirString;
    this->useChecks = useChecks;
    this->logFile = logFile;
}

short int SwiftyListParams::getVerbose() {
    return this->verbose;
}

bool SwiftyListParams::getChecks() {
    return this->useChecks;
}

ListStringView SwiftyListParams::getLogDir() {
    return this->logDir;
}

FILE *SwiftyListParams::getLogFile() {
    return this->logFile;
}

ListOpResult SwiftyList::remove(size_t pos) {
    return this->pop(pos, nullptr);
}

ListOpResult SwiftyList::swap(size_t firstPos, size_t secondPos) {
    if (firstPos >= this->size || secondPos >= this->size )
        return LIST_OP_OVERFLOW;
    size_t first  = firstPos;
    size_t second = secondPos;
    if (!this->optimized){
        size_t iterator = 0;
        for (size_t i = 0; i <= firstPos; i++) {
            iterator = this->storage[iterator].next;
        }
        first = iterator;
    } else {
        first++;
    }

    if (!this->optimized){
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

ListOpResult SwiftyList::pop(size_t pos, ListElem *value) {
    PERFORM_CHECKS;
    if (pos >= this->size)
        return LIST_OP_UNDERFLOW;
    if (!this->optimized){
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


ListOpResult SwiftyList::popBack(ListElem *value) {
    return this->pop(this->size - 1, value);
}

ListOpResult SwiftyList::popFront(ListElem *value) {
    return this->pop(0, value);
}

ListOpResult SwiftyList::optimize() {
    PERFORM_CHECKS;

    for (size_t i = 0; i < this->size; i++) {
        size_t nextPos = this->storage[i].next - 1;
        if (i < nextPos)
            this->swapPhysicOnly(i, nextPos);
    }
    this->optimized = true;
    return LIST_OP_OK;
}

bool SwiftyList::isOptimized() {
    return this->optimized;
}

bool SwiftyList::isEmpty() {
    return this->size == 0;
}

SwiftyList::~SwiftyList() {
    free(this->storage);
}

ListOpResult SwiftyList::checkUp() {
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

void SwiftyList::swapPhysicOnly(size_t firstPos, size_t secondPos) {
    firstPos++;
    secondPos++;
    if (secondPos == firstPos)
        return;
    this->optimized = false;

    if (this->storage[firstPos].previous == secondPos) {
        size_t tmpPos = firstPos;
        firstPos = secondPos;
        secondPos = tmpPos;
    }

    const SwiftyListNode tmpFirst = this->storage[firstPos];
    this->storage[firstPos] = this->storage[secondPos];
    this->storage[secondPos] = tmpFirst;
    
    this->storage[tmpFirst.previous].next = secondPos;
    this->storage[this->storage[firstPos].next].previous = firstPos;
    
    if (this->storage[firstPos].previous == firstPos ||
        tmpFirst.next == secondPos ||
            this->storage[firstPos].next == firstPos ||
        tmpFirst.previous == secondPos) {
        this->storage[secondPos].next = firstPos;
        this->storage[firstPos].previous = secondPos;
    } else {
        this->storage[tmpFirst.next].previous = secondPos;
        this->storage[this->storage[firstPos].previous].next = firstPos;
    }
    
}

ListOpResult SwiftyList::deOptimize(){
    PERFORM_CHECKS;
    for (size_t i = 0; i < this->size; i++) {
        int rPos = rand();
        int lPos = rand();
        this->swapPhysicOnly(rPos % this->size, lPos % this->size);
    }
    return LIST_OP_OK;
}

ListOpResult SwiftyList::shrinkToFit() {
    return this->resize(0);
}

#undef INITIAL_INCREASE
