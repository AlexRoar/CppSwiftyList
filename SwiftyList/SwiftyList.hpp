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

typedef int ListElem;

#define DOTPATH "/usr/local/bin/dot"

enum ListOpResult {
    LIST_OP_OK,
    LIST_OP_NOMEM,
    LIST_OP_CORRUPTED,
    LIST_OP_OVERFLOW,
    LIST_OP_UNDERFLOW,
    LIST_OP_NOTFOUND
};

/**
 * Verbose levels:
 * 0 - No dump at all
 * 1 - Dump only when requested or fail occures
 * 2 - Dump almost all operations
 */
struct _ListStringView {
    char*  storage;
    size_t len;
};

struct SwiftyListParams {
    //private:
    short int verbose;
    bool            useChecks;
    _ListStringView logDir;
    FILE*           logFile;
    //public:
    short int       getVerbose();
    bool            getChecks();
    _ListStringView getLogDir();
    FILE*           getLogFile();
    
    SwiftyListParams(short int verbose, char* logDir, bool useChecks, FILE * logFile);
    
    ~SwiftyListParams();
};

struct SwiftyListNode {
    ListElem value;
    size_t  next;
    size_t  previous;
};

struct SwiftyList {
    //private:
    SwiftyListNode*    storage;
    bool               optimized;
    size_t             capacity;
    size_t             size;
    SwiftyListParams*  params;
    
    size_t head;
    size_t tail;
    
    size_t freeSize;
    size_t freeSpace;
    
    size_t sumSize();
    
    ListOpResult firstElementInsertion(const ListElem value, size_t* pos);
    size_t       getFreePos();
    ListOpResult reallocate  (bool onlyUp);
    ListOpResult insertAside (const ListElem value, size_t* pos);
    void         swapValues(size_t firstPos, size_t secondPos);
    void         swapPhysicOnly(size_t firstPos, size_t secondPos);
    void         addFreedSpace(size_t pos);
    //public:
    ListOpResult set         (size_t pos,  const ListElem value);
    ListOpResult get         (size_t pos,       ListElem* value);
    ListOpResult pushFront   (const ListElem value);
    ListOpResult pushBack    (const ListElem value);
    ListOpResult popFront    (ListElem* value);
    ListOpResult popBack     (ListElem* value);
    ListOpResult search      (size_t* pos, const ListElem value);
    ListOpResult remove      (size_t pos);
    ListOpResult resize      (size_t nElements);
    ListOpResult swap        (size_t firstPos, size_t secondPos);
    ListOpResult clear       ();
    ListOpResult optimize    ();
    ListOpResult checkUp     ();
    ListOpResult print       ();
    
    size_t getSize();
    size_t getCapacity();
    
    bool isOptimized();
    bool isEmpty();
    
    SwiftyList(size_t initialSize, short int verbose, char* logDir, FILE * logFile, bool useChecks);
    ~SwiftyList();
};

#endif /* SwiftyList_hpp */
