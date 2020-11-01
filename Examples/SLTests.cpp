//
// Created by Александр Дремов on 01.11.2020.
//

#include "gtest/gtest.h"
#include <stdlib.h>
#include "SwiftyList.hpp"
#include "GraphVizDumper.hpp"

TEST(SwiftyListTests, checkUp) {
    SwiftyList* list = new SwiftyList(0, 0, NULL, NULL, false);
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    list->pushBack(1);
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    for(size_t i = 0; i < 100; i++) {
        list->pushFront(i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);

    size_t tmpNext = list->storage[4].next;
    list->storage[4].next = 4;
    EXPECT_TRUE(list->checkUp() == LIST_OP_CORRUPTED);
    list->storage[4].next = tmpNext;
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);

    list->storage[2].next = 4;
    EXPECT_TRUE(list->checkUp() == LIST_OP_CORRUPTED);
}


TEST(SwiftyListTests, pushBack) {
    SwiftyList* list = new SwiftyList(0, 0, NULL, NULL, false);
    for(size_t i = 0; i < 100; i++) {
        list->pushBack(i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    EXPECT_EQ(list->isOptimized(), true);
    for(size_t i = 0; i < 100; i++) {
        EXPECT_EQ(list->storage[1 + i].value, i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, pushFront) {
    SwiftyList* list = new SwiftyList(0, 0, NULL, NULL, 0);
    for(size_t i = 0; i < 100; i++) {
        list->pushFront(i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    for(size_t i = 99; ; i--) {
        EXPECT_EQ(list->storage[1 + i].value, i);
        if (i == 0)
            break;
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, popFront) {
    SwiftyList* list = new SwiftyList(0, 0, NULL, NULL, 0);
    for(size_t i = 0; i < 100; i++) {
        list->pushBack(i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    for(size_t i = 0; i < 100; i++) {
        int value = 0;
        list->popFront(&value);
        EXPECT_EQ(value, i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, popBack) {
    SwiftyList* list = new SwiftyList(0, 0, NULL, NULL, 0);
    for(size_t i = 0; i < 100; i++) {
        list->pushFront(i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    for(size_t i = 0; i < 100; i++) {
        int value = 0;
        list->popBack(&value);
        EXPECT_EQ(value, i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, setGet) {
    SwiftyList* list = new SwiftyList(0, 0, NULL, NULL, 0);
    for(size_t i = 0; i < 100; i++) {
        list->pushFront(i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    for(size_t i = 0; i < 100; i++) {
        int value = rand();
        list->set(i, value);
        int gValue = 0;
        list->get(i, &gValue);
        EXPECT_EQ(value, gValue);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, search) {
    SwiftyList* list = new SwiftyList(0, 0, NULL, NULL, 0);
    for(size_t i = 0; i < 100; i++) {
        list->pushFront(2*i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    for(size_t i = 0; i < 100; i++) {
        size_t pos = 0;
        list->search(&pos, 2*i);
        EXPECT_EQ(pos, i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, relloacations) {
    SwiftyList* list = new SwiftyList(10, 0, NULL, NULL, false);
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    for(size_t i = 0; i < 10; i++) {
        list->pushBack(i);
    }

    EXPECT_EQ(list->getCapacity(), 10);
    list->pushBack(0);
    EXPECT_EQ(list->getCapacity(), 20);
    int tmpVal = 0;
    list->popBack(&tmpVal);
    EXPECT_EQ(list->getCapacity(), 20);
    for(size_t i = 0; i < 10; i++) {
        list->popBack(&tmpVal);
    }
    EXPECT_EQ(list->getCapacity(), 20);
}

TEST(SwiftyListTests, remove) {
    SwiftyList* list = new SwiftyList(0, 0, NULL, NULL, 0);
    for(size_t i = 0; i < 100; i++) {
        list->pushBack(i);
    }
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    list->remove(0);

    int val = 0;
    list->get(0, &val);

    EXPECT_EQ(val, 1);
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);

    list->remove(3);
    list->get(3, &val);

    EXPECT_EQ(val, 5);
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, sideCases) {
    SwiftyList* list = new SwiftyList(0, 0, NULL, NULL, 0);
    list->pushBack(1);
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);

    int val = 0;
    list->get(0, &val);
    EXPECT_TRUE(list->get(0, &val) == LIST_OP_OK);
    EXPECT_EQ(val, 1);

    list->remove(0);
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    EXPECT_TRUE(list->get(0, &val) != LIST_OP_OK);

    EXPECT_TRUE(list->remove(0) != LIST_OP_OK);
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);

    list->pushFront(1);
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, clear) {
    SwiftyList* list = new SwiftyList(10, 0, NULL, NULL, false);
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    for(size_t i = 0; i < 10; i++) {
        list->pushBack(i);
    }

    list->clear();
    EXPECT_TRUE(list->checkUp() == LIST_OP_OK);
    EXPECT_TRUE(list->getSize() == 0);
}

TEST(SwiftyListTests, physicSwap) {
    const int testSize = 10;
    SwiftyList* list = new SwiftyList(0, 0, NULL, NULL, 0);
    for(size_t i = 0; i < testSize; i++) {
        list->pushBack(i);
    }

    for(size_t i = 0; i < 5000; i++) {
        int rPos = rand();
        int lPos = rand();
//        ListGraphDumper* dumper = new ListGraphDumper(list, (char*)"grapfTst.gv", (char*)"grTestPre.png");
//        dumper->build();
        list->swapPhysicOnly(rPos % testSize, lPos % testSize);
        int tmp = 0;
        for(size_t j = 0; j < testSize; j++) {
            list->get(j, &tmp);
            EXPECT_EQ(tmp, j);
            if (tmp != j){
//                ListGraphDumper* dumper = new ListGraphDumper(list, (char*)"grapfTst.gv", (char*)"grTest.png");
//                dumper->build();
                printf("%d <-> %d\n", rPos% testSize, lPos% testSize);
                return;
            }
        }
        if (tmp != testSize - 1)
            break;
    }
}