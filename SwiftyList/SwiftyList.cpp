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
head(0),
tail(0),
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
    this->head = 0;
    this->tail = 0;
    return LIST_OP_OK;
}


ListOpResult SwiftyList::resize(size_t nElements) {
    PERFORM_CHECKS;
    size_t sumSizes = this->size;
    if (nElements < sumSizes) {
        nElements = sumSizes;
    }
    
    SwiftyListNode *newStorage = (SwiftyListNode *) realloc(this->storage, nElements * sizeof(SwiftyListNode));
    
    if (newStorage == nullptr) return LIST_OP_NOMEM;
    
    this->storage = newStorage;
    this->capacity = nElements;
    return LIST_OP_OK;
}

ListOpResult SwiftyList::insertAside(const ListElem value, size_t *pos) {
    *pos = this->getFreePos();
    if (*pos == 0)
        return LIST_OP_NOMEM;
    this->storage[this->tail].next = *pos;
    this->storage[this->head].previous = *pos;
    this->storage[*pos].value = value;
    this->storage[*pos].previous = this->tail;
    this->storage[*pos].next = this->head;
    return LIST_OP_OK;
}

ListOpResult SwiftyList::pushBack(const ListElem value) {
    PERFORM_CHECKS;
    size_t pos = 0;
    if (this->size == 0) {
        return firstElementInsertion(value, &pos);
    } else {
        ListOpResult insertRes = this->insertAside(value, &pos);
        if (insertRes != LIST_OP_OK)
            return insertRes;
        this->tail = pos;
    }
    this->size++;
    return LIST_OP_OK;
}

ListOpResult SwiftyList::pushFront(const ListElem value) {
    PERFORM_CHECKS;
    this->optimized = false;
    size_t pos = 0;
    if (this->size == 0) {
        return firstElementInsertion(value, &pos);
    } else {
        ListOpResult insertRes = this->insertAside(value, &pos);
        if (insertRes != LIST_OP_OK)
            return insertRes;
        this->head = pos;
    }
    this->size++;
    return LIST_OP_OK;
}

size_t SwiftyList::getFreePos() {
    ListOpResult reallocRes = this->reallocate(true);
    if (reallocRes != LIST_OP_OK)
        return 0;
    return  this->size + 1;
}

ListOpResult SwiftyList::firstElementInsertion(const ListElem value, size_t *pos) {
    *pos = this->getFreePos();
    if (*pos == 0)
        return LIST_OP_NOMEM;
    this->tail = *pos;
    this->head = *pos;
    this->size = 1;
    this->storage[*pos].value = value;
    this->storage[*pos].next = *pos;
    this->storage[*pos].previous = *pos;
    return LIST_OP_OK;
}

ListOpResult SwiftyList::get(size_t pos, ListElem *value) {
    PERFORM_CHECKS;
    if (pos >= this->size)
        return LIST_OP_UNDERFLOW;
    if (this->optimized) {
        *value = this->storage[pos + 1].value;
    } else {
        size_t iterator = this->head;
        for (size_t i = 0; i < pos; i++) {
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
        size_t iterator = this->head;
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
    
    SwiftyListNode *newStorage = (SwiftyListNode *) realloc(this->storage, (newCapacity + 1) * sizeof(SwiftyListNode));
    if (newStorage == NULL)
        return LIST_OP_NOMEM;
    this->storage = newStorage;
    this->capacity = newCapacity;
    return LIST_OP_OK;
}

ListOpResult SwiftyList::print() {
    PERFORM_CHECKS;
    size_t pos = this->head;
    if (pos == 0 || this->size == 0)
        return LIST_OP_OK;
    while (true) {
        printf("%d ", this->storage[pos].value);
        if (pos == this->tail)
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
    *pos = this->head;
    while (true) {
        if (this->storage[*pos].value == value) {
            (*pos)--;
            return LIST_OP_OK;
        }
        
        if (*pos == this->tail)
            break;
        *pos = this->storage[*pos].next;
    }
    return LIST_OP_NOTFOUND;
}

SwiftyListParams::SwiftyListParams(short int verbose, char *logDir, bool useChecks, FILE *logFile) {
    this->verbose = verbose;
    _ListStringView logDirString = {};
    if (logDir != NULL)
        logDirString.len = strlen(logDir);
    logDirString.storage = logDir;
    this->logDir = logDirString;
    this->useChecks = useChecks;
    this->logFile = logFile;
}

ListOpResult SwiftyList::remove(size_t pos) {
    PERFORM_CHECKS;
    if (pos >= this->size)
        return LIST_OP_UNDERFLOW;
    
    if (this->size == 1)
        return this->clear();
    
    if (pos == 0) {
        ListElem tmp = 0;
        return this->popFront(&tmp);
    } else if (pos + 1 == this->size) {
        ListElem tmp = 0;
        return this->popBack(&tmp);
    }
    
    size_t iterator = this->head;
    for (size_t i = 0; i < pos; i++) {
        iterator = this->storage[iterator].next;
    }
    
    this->swapPhysicOnly(iterator - 1, this->size - 1);
    iterator = this->size;
    
    size_t nextInd = this->storage[iterator].next;
    size_t prevInd = this->storage[iterator].previous;
    this->storage[prevInd].next = nextInd;
    this->storage[nextInd].previous = prevInd;
    this->size--;
    return LIST_OP_OK;
}

void SwiftyList::swapValues(size_t firstPos, size_t secondPos) {
    firstPos++;
    secondPos++;
    
    ListElem tmp = this->storage[firstPos].value;
    this->storage[firstPos].value = this->storage[secondPos].value;
    this->storage[secondPos].value = tmp;
}

ListOpResult SwiftyList::popBack(ListElem *value) {
    PERFORM_CHECKS;
    if (this->size == 0)
        return LIST_OP_UNDERFLOW;
    this->swapPhysicOnly(this->tail - 1, this->size - 1);
    this->tail = this->size;
    size_t lastTail = this->tail;
    *value = this->storage[lastTail].value;
    size_t prevTail = this->storage[lastTail].previous;
    
    if (this->size == 1) {
        this->clear();
        return LIST_OP_OK;
    }
    
    this->tail = prevTail;
    
    this->storage[prevTail].next = this->head;
    this->storage[this->head].previous = prevTail;
    
    this->size--;
    
    this->reallocate(false);
    return LIST_OP_OK;
}

ListOpResult SwiftyList::popFront(ListElem *value) {
    PERFORM_CHECKS;
    this->optimized = false;
    if (this->size == 0)
        return LIST_OP_UNDERFLOW;
    this->swapPhysicOnly(this->head - 1, this->size -1);
    this->head = this->size;
    size_t lastHead = this->head;
    *value = this->storage[lastHead].value;
    size_t nextHead = this->storage[lastHead].next;
    
    if (this->size == 1) {
        this->clear();
        return LIST_OP_OK;
    }
    
    this->head = nextHead;
    
    this->storage[nextHead].previous = this->tail;
    this->storage[this->tail].next = nextHead;
    
    this->size--;
    this->reallocate(false);
    
    return LIST_OP_OK;
}

ListOpResult SwiftyList::optimize() {
    PERFORM_CHECKS;
    size_t sumSize = this->size;
    this->swapPhysicOnly(0, this->head - 1);
    for (size_t i = 1; i <= sumSize; i++) {
        size_t nextPos = this->storage[i].next - 1;
        if (i < nextPos)
            this->swapPhysicOnly(i, nextPos);
    }
    this->head = 1;
    this->tail = this->size;
    this->optimized = true;
    return LIST_OP_OK;
}

ListOpResult SwiftyList::swap(size_t firstPos, size_t secondPos) {
    PERFORM_CHECKS;
    if (firstPos + 1 >= this->size || secondPos + 1 >= this->size)
        return LIST_OP_UNDERFLOW;
    size_t first = 0;
    size_t second = 0;
    
    if (this->optimized) {
        first = firstPos + 1;
        second = secondPos + 1;
    } else {
        first = this->head;
        for (size_t i = 0; i < firstPos; i++) {
            first = this->storage[first].next;
        }
        
        second = this->head;
        for (size_t i = 0; i < firstPos; i++) {
            second = this->storage[second].next;
        }
    }
    
    size_t firstPrev = this->storage[first].previous;
    size_t firstNext = this->storage[first].next;
    
    size_t secondPrev = this->storage[second].previous;
    size_t secondNext = this->storage[second].next;
    
    this->storage[firstPrev].next = second;
    this->storage[firstNext].previous = second;
    
    this->storage[secondPrev].next = first;
    this->storage[secondNext].previous = first;
    
    this->storage[first].previous = secondPrev;
    this->storage[first].next = secondNext;
    
    this->storage[second].previous = firstPrev;
    this->storage[second].next = firstNext;
    
    if (this->tail == first) {
        this->tail = second;
    } else if (this->tail == second) {
        this->tail = first;
    }
    
    if (this->head == second) {
        this->head = first;
    } else if (this->head == first) {
        this->head = second;
    }
    
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
    size_t pos = this->head;
    
    if (this->size == 0)
        return LIST_OP_OK;
    
    for (size_t i = 0; i < this->size; i++) {
        SwiftyListNode node = this->storage[pos];
        if ((node.next == pos || node.previous == pos) && this->size > 1)
            return LIST_OP_CORRUPTED;
        if (pos == this->tail) {
            if (i + 1 != this->size)
                return LIST_OP_CORRUPTED;
        }
        pos = node.next;
    }
    
    if (pos != this->head)
        return LIST_OP_CORRUPTED;
    
    return LIST_OP_OK;
}

void SwiftyList::swapPhysicOnly(size_t firstPos, size_t secondPos) {
    firstPos++;
    secondPos++;
    if (secondPos == firstPos)
        return;
    this->optimized = false;
    if (firstPos > secondPos || this->storage[firstPos].next == firstPos) {
        size_t tmp = firstPos;
        firstPos = secondPos;
        secondPos = tmp;
    }

    if (this->storage[firstPos].previous == secondPos) {
        size_t tmp = firstPos;
        firstPos = secondPos;
        secondPos = tmp;
    }
    
    const SwiftyListNode tmpFirst = this->storage[firstPos];
    this->storage[firstPos] = this->storage[secondPos];
    this->storage[secondPos] = tmpFirst;
    const size_t tmpHead = this->head;
    const size_t tmpTail = this->tail;
    if ((firstPos == this->head && secondPos == this->tail) ||
        (secondPos == this->head && firstPos == this->tail)) {
        this->head = this->tail;
        this->tail = tmpHead;
    } else {
        if (tmpHead == firstPos) {
            this->head = secondPos;
        } else if (tmpHead == secondPos) {
            this->head = firstPos;
        }
        if (tmpTail == secondPos) {
            this->tail = firstPos;
        } else if (tmpTail == firstPos) {
            this->tail = secondPos;
        }
    }
    
    this->storage[tmpFirst.previous].next = secondPos;
    this->storage[this->storage[firstPos].next].previous = firstPos;
    
    if (this->storage[firstPos].previous == firstPos ||
        tmpFirst.next == secondPos ||
            this->storage[firstPos].next == firstPos ||
        tmpFirst.previous == secondPos) {
        this->storage[secondPos].next = firstPos;
        this->storage[firstPos].previous = secondPos;
        if (this->size == 2){
            this->storage[firstPos].next = secondPos;
            this->storage[secondPos].previous = firstPos;
        }
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

#undef INITIAL_INCREASE
