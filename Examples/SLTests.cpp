//
// Created by Александр Дремов on 01.11.2020.
//

#include "gtest/gtest.h"
#include <stdlib.h>
#include <string.h>
#include "SwiftyList.hpp"

TEST(SwiftyListTests, checkUp) {
    SwiftyList<int> list(0, 0, NULL, NULL, false);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    list.pushBack(1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 100; i++) {
        list.pushFront(i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    size_t tmpNext = list.storage[4].next;
    list.storage[4].next = 4;
    EXPECT_TRUE(list.checkUp() == LIST_OP_CORRUPTED);
    list.storage[4].next = tmpNext;
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    list.storage[2].next = 4;
    EXPECT_TRUE(list.checkUp() == LIST_OP_CORRUPTED);
}


TEST(SwiftyListTests, pushBack) {
    SwiftyList<int> list(0, 0, NULL, NULL, false);
    for (size_t i = 0; i < 100; i++) {
        list.pushBack(i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    EXPECT_EQ(list.isOptimized(), true);
    for (size_t i = 0; i < 100; i++) {
        EXPECT_EQ(list.storage[1 + i].value, i);
    }
    for (size_t i = 0; i < 100; i++) {
        int val = 0;
        list.get(i, &val);
        EXPECT_EQ(val, i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, pushFront) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    for (size_t i = 0; i < 100; i++) {
        list.pushFront(i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 99;; i--) {
        EXPECT_EQ(list.storage[1 + i].value, i);
        if (i == 0)
            break;
    }
    for (size_t i = 0; i < 100; i++) {
        int val = 0;
        list.get(i, &val);
        EXPECT_EQ(val, 100 - i - 1);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, popFront) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    for (size_t i = 0; i < 100; i++) {
        list.pushBack(i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 100; i++) {
        int value = 0;
        list.popFront(&value);
        EXPECT_EQ(value, i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, popBack) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    for (size_t i = 0; i < 100; i++) {
        list.pushFront(i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 100; i++) {
        int value = 0;
        list.popBack(&value);
        EXPECT_EQ(value, i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, setGet) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    for (size_t i = 0; i < 100; i++) {
        list.pushFront(i);
    }
    list.deOptimize();
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 100; i++) {
        int value = rand();
        list.set(i, value);
        int gValue = 0;
        list.get(i, &gValue);
        EXPECT_EQ(value, gValue);
    }

    list.optimize();

    for (size_t i = 0; i < 100; i++) {
        int value = rand();
        list.set(i, value);
        int gValue = 0;
        list.get(i, &gValue);
        EXPECT_EQ(value, gValue);
    }

    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, search) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    for (size_t i = 0; i < 100; i++) {
        list.pushFront(2 * i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 100; i++) {
        size_t pos = 0;
        list.search(&pos, 2 * i);
        EXPECT_EQ(pos, i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, relloacations) {
    SwiftyList<int> list(10, 0, NULL, NULL, false);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 10; i++) {
        list.pushBack(i);
    }

    EXPECT_EQ(list.getCapacity(), 10);
    list.pushBack(0);
    EXPECT_EQ(list.getCapacity(), 20);
    int tmpVal = 0;
    list.popBack(&tmpVal);
    EXPECT_EQ(list.getCapacity(), 20);
    for (size_t i = 0; i < 5; i++) {
        list.popBack(&tmpVal);
    }
    EXPECT_EQ(list.getCapacity(), 10);
}

TEST(SwiftyListTests, remove) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    for (size_t i = 0; i < 10; i++) {
        list.pushBack(i);
    }
    // 0 1 2 3 4 5 6 7 8 9
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    list.remove(0);

    // 1 2 3 4 5 6 7 8 9
    int val = 0;
    list.get(0, &val);

    EXPECT_EQ(val, 1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    // 1 2 3 5 6 7 8 9
    list.remove(3);
    list.get(3, &val);

    EXPECT_EQ(val, 5);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, sideCases) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    list.pushBack(1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    int val = 0;
    list.get(0, &val);
    EXPECT_TRUE(list.get(0, &val) == LIST_OP_OK);
    EXPECT_EQ(val, 1);

    list.remove(0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    EXPECT_TRUE(list.get(0, &val) != LIST_OP_OK);

    EXPECT_TRUE(list.remove(0) != LIST_OP_OK);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    list.pushFront(1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, clear) {
    SwiftyList<int> list(0, 0, NULL, NULL, false);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 10; i++) {
        list.pushBack(i);
    }

    list.clear();
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    EXPECT_TRUE(list.getSize() == 0);
}

TEST(SwiftyListTests, physicSwap) {
    for (int testSize = 1; testSize < 100; testSize++) {
        SwiftyList<int> list(0, 0, NULL, NULL, 0);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            list.pushBack(i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

        for (size_t i = 0; i < 10; i++) {
            int rPos = rand();
            int lPos = rand();
//        ListGraphDumper* dumper = new ListGraphDumper(list, (char*)"grapfTst.gv", (char*)"grTestPre.png");
//        dumper->build();
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
            list.swapPhysicOnly(rPos % testSize, lPos % testSize);
            if (!(list.checkUp() == LIST_OP_OK)) {
                printf("%d: %d <-> %d\n", testSize, rPos % testSize, lPos % testSize);
            }
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
            int tmp = 0;
            for (size_t j = 0; j < testSize; j++) {
                list.get(j, &tmp);
                EXPECT_EQ(tmp, j);
                if (tmp != j) {
//                ListGraphDumper* dumper = new ListGraphDumper(list, (char*)"grapfTst.gv", (char*)"grTest.png");
//                dumper->build();
                    printf("%d <-> %d\n", rPos % testSize, lPos % testSize);
                    return;
                }
            }
            if (tmp != testSize - 1)
                break;
        }
    }
}

TEST(SwiftyListTests, optimizer) {
    for (int testSize = 1; testSize < 100; testSize++) {
        SwiftyList<int> list(0, 0, NULL, NULL, 0);
        for (size_t i = 0; i < testSize; i++) {
            list.pushBack(i);
        }

        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < 5000; i++) {
            int rPos = rand();
            int lPos = rand();
            list.swapPhysicOnly(rPos % testSize, lPos % testSize);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        list.optimize();
        EXPECT_TRUE(list.isOptimized());
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            EXPECT_EQ(list.storage[i + 1].value, i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        EXPECT_TRUE(list.isOptimized());
    }
}

TEST(SwiftyListTests, insert) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    list.optimized = false;
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    list.insert(0, 5);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    int tmp = 0;
    list.get(0, &tmp);


    list.insert(0, 5);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    tmp = 0;
    list.get(0, &tmp);
    EXPECT_EQ(tmp, 5);

    tmp = 0;
    list.get(1, &tmp);
    EXPECT_EQ(tmp, 5);
}

TEST(SwiftyListTests, empty) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    EXPECT_EQ(0, list.getSize());
    EXPECT_TRUE(list.isEmpty());
    EXPECT_TRUE(list.isOptimized());
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, pushPop) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 1000; i++) {
        if (i % 2 == 0)
            list.pushBack(i);
        else
            list.pushFront(i);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
    for (size_t i = 0; i < 1000; i++) {
        int val = 0;
        if (i % 2 == 0)
            list.popBack(&val);
        else
            list.popFront(&val);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
    EXPECT_TRUE(list.isEmpty());
}

TEST(SwiftyListTests, randomRemove) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 1000; i++) {
        if (i % 2 == 0)
            list.pushBack(i);
        else
            list.pushFront(i);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
    for (size_t i = 0; i < 1000; i++) {
        list.remove(rand() % list.getSize());
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
    EXPECT_TRUE(list.isEmpty());
}

TEST(SwiftyListTests, capacity) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 1000; i++) {
        if (i % 2 == 0)
            list.pushBack(i);
        else
            list.pushFront(i);
        EXPECT_GE(list.getCapacity(), i + 1);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, resize0) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 1000; i++) {
        if (i % 2 == 0)
            list.pushBack(i);
        else
            list.pushFront(i);
        list.resize(0);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        EXPECT_GE(list.getCapacity(), i + 1);
    }

    list.resize(0);
    EXPECT_GE(list.getCapacity(), list.getSize());

    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, resizeALot) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 1000; i++) {
        if (i % 2 == 0)
            list.pushBack(i);
        else
            list.pushFront(i);
        list.resize(list.getSize() * 2);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        EXPECT_GE(list.getCapacity(), i + 1);
    }

    list.resize(list.getSize() * 2);
    EXPECT_GE(list.getCapacity(), list.getSize());

    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, clearBig) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 1000; i++) {
        if (i % 2 == 0)
            list.pushBack(i);
        else
            list.pushFront(i);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }

    list.clear();
    EXPECT_GE(list.getCapacity(), list.getSize());
    EXPECT_TRUE(list.isEmpty());
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, shrinkToFit) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 1000; i++) {
        if (i % 2 == 0)
            list.pushBack(i);
        else
            list.pushFront(i);
        list.shrinkToFit();
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }

    list.shrinkToFit();
    EXPECT_GE(list.getCapacity(), list.getSize());
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, deoptimize) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 1000; i++) {
        if (i % 2 == 0)
            list.pushBack(i);
        else
            list.pushFront(i);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }

    list.deOptimize();
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, swap) {
    SwiftyList<int> list(0, 0, NULL, NULL, 0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 1000; i++) {
        list.pushBack(i);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }

    for (size_t i = 0; i < 5000; i++) {
        int firstElem = 0;
        int secondElem = 0;
        int firstElemInit = 0;
        int secondElemInit = 0;

        const int firstPos = rand() % list.getSize();
        const int secondPos = rand() % list.getSize();

        list.get(firstPos, &firstElemInit);
        list.get(secondPos, &secondElemInit);

        list.swap(firstPos, secondPos);

        list.get(firstPos, &firstElem);
        list.get(secondPos, &secondElem);

        EXPECT_EQ(firstElem, secondElemInit);
        EXPECT_EQ(firstElemInit, secondElem);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }

    list.deOptimize();

    for (size_t i = 0; i < 5000; i++) {
        int firstElem = 0;
        int secondElem = 0;
        int firstElemInit = 0;
        int secondElemInit = 0;

        const int firstPos = rand() % list.getSize();
        const int secondPos = rand() % list.getSize();

        list.get(firstPos, &firstElemInit);
        list.get(secondPos, &secondElemInit);

        list.swap(firstPos, secondPos);

        list.get(firstPos, &firstElem);
        list.get(secondPos, &secondElem);

        EXPECT_EQ(firstElem, secondElemInit);
        EXPECT_EQ(firstElemInit, secondElem);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, getters) {

    FILE* logFile = fopen("/dev/null", "w");
    SwiftyList<int> list(10, 2, (char*)"/log", logFile, false);
    EXPECT_TRUE(strcmp("/log", list.params->getLogDir().storage) == 0);
    EXPECT_TRUE(list.params->getLogFile() == logFile );
    EXPECT_TRUE(list.params->getChecks() == false );
    EXPECT_TRUE(list.params->getVerbose() == 2 );
}