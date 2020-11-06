//
// Created by Александр Дремов on 01.11.2020.
//

#include "gtest/gtest.h"
#include <stdlib.h>
#include <string.h>
#include "SwiftyList.hpp"

#define CAPACITY_RANGE 500

TEST(SwiftyListTests, checkUp) {
    SwiftyList<int> list(0, 0, NULL, false);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    list.pushBack(1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 100; i++) {
        list.pushFront(i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 100; i++) {
        list.popFront(NULL);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);;
}


TEST(SwiftyListTests, pushBack) {
    SwiftyList<int> list(0, 0, NULL, false);
    for (size_t i = 0; i < 100; i++) {
        list.pushBack(i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    EXPECT_EQ(list.isOptimized(), true);
    for (size_t i = 0; i < 100; i++) {
        int tmp = 0;
        list.getLogic(i, &tmp);
        EXPECT_EQ(tmp, i);
    }
    for (size_t i = 0; i < 100; i++) {
        int val = 0;
        list.getLogic(i, &val);
        EXPECT_EQ(val, i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, pushFront) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        for (size_t i = 0; i < testSize; i++) {
            EXPECT_TRUE(list.pushFront(i) == LIST_OP_OK);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            int tmp = 0;
            EXPECT_TRUE(list.getLogic(i, &tmp) == LIST_OP_OK);
            EXPECT_EQ(tmp, testSize - i - 1);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, popFront) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        for (size_t i = 0; i < testSize; i++) {
            list.pushBack(i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            int value = 0;
            list.popFront(&value);
            EXPECT_EQ(value, i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, popBack) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        for (size_t i = 0; i < testSize; i++) {
            list.pushFront(i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            int value = 0;
            list.popBack(&value);
            EXPECT_EQ(value, i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, setGetLogic) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        for (size_t i = 0; i < testSize; i++) {
            list.pushFront(i);
        }
        list.deOptimize();
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            int value = rand();
            list.setLogic(i, value);
            int gValue = 0;
            list.getLogic(i, &gValue);
            EXPECT_EQ(value, gValue);
        }

        list.optimize();

        for (size_t i = 0; i < testSize; i++) {
            int value = rand();
            list.setLogic(i, value);
            int gValue = 0;
            list.getLogic(i, &gValue);
            EXPECT_EQ(value, gValue);
        }

        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, search) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        for (size_t i = 0; i < testSize; i++) {
            list.pushFront(2 * i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            size_t pos = 0;
            list.search(&pos, 2 * i);
            EXPECT_EQ(pos, i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, relloacations) {
    SwiftyList<int> list(10, 0, NULL, false);
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
}

TEST(SwiftyListTests, removeLogic) {
    SwiftyList<int> list(0, 0, NULL, 0);
    for (size_t i = 0; i < 10; i++) {
        list.pushBack(i);
    }
    // 0 1 2 3 4 5 6 7 8 9
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    list.removeLogic(0);

    // 1 2 3 4 5 6 7 8 9
    int val = 0;
    list.getLogic(0, &val);

    EXPECT_EQ(val, 1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    // 1 2 3 5 6 7 8 9
    list.removeLogic(3);
    list.getLogic(3, &val);

    EXPECT_EQ(val, 5);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, sideCases) {
    SwiftyList<int> list(0, 0, NULL, 0);
    list.pushBack(1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    int val = 0;
    list.getLogic(0, &val);
    EXPECT_TRUE(list.getLogic(0, &val) == LIST_OP_OK);
    EXPECT_EQ(val, 1);

    list.removeLogic(0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    EXPECT_TRUE(list.getLogic(0, &val) != LIST_OP_OK);

    EXPECT_TRUE(list.removeLogic(0) != LIST_OP_OK);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    list.pushFront(1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, clear) {
    SwiftyList<int> list(0, 0, NULL, false);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 10; i++) {
        list.pushBack(i);
    }

    list.clear();
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    EXPECT_TRUE(list.getSize() == 0);
}

TEST(SwiftyListTests, insertLogic) {
    SwiftyList<int> list(0, 0, NULL, 0);
    list.deOptimize();
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    list.insertAfter(0, 5, nullptr);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    int tmp = 0;
    list.getLogic(0, &tmp);


    list.insertAfter(0, 5, nullptr);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    tmp = 0;
    list.getLogic(0, &tmp);
    EXPECT_EQ(tmp, 5);

    tmp = 0;
    list.getLogic(1, &tmp);
    EXPECT_EQ(tmp, 5);
}

TEST(SwiftyListTests, empty) {
    SwiftyList<int> list(0, 0, NULL, 0);
    EXPECT_EQ(0, list.getSize());
    EXPECT_TRUE(list.isEmpty());
    EXPECT_TRUE(list.isOptimized());
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, pushPop) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            if (i % 2 == 0)
                list.pushBack(i);
            else
                list.pushFront(i);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }
        for (size_t i = 0; i < testSize; i++) {
            int val = 0;
            if (i % 2 == 0)
                list.popBack(&val);
            else
                list.popFront(&val);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }
        EXPECT_TRUE(list.isEmpty());
    }
}

TEST(SwiftyListTests, randomRemove) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            if (i % 2 == 0)
                list.pushBack(i);
            else
                list.pushFront(i);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }
        for (size_t i = 0; i < testSize; i++) {
            list.removeLogic(rand() % list.getSize());
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }
        EXPECT_TRUE(list.isEmpty());
    }
}

TEST(SwiftyListTests, capacity) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            if (i % 2 == 0)
                list.pushBack(i);
            else
                list.pushFront(i);
            EXPECT_GE(list.getCapacity(), i + 1);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, clearBig) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
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
}

TEST(SwiftyListTests, deoptimize) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            if (i % 2 == 0)
                list.pushBack(i);
            else
                list.pushFront(i);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }

        list.deOptimize();
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, swapLogic) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, 0);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            list.pushBack(i);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }

        for (size_t i = 0; i < testSize; i++) {
            int firstElem = 0;
            int secondElem = 0;
            int firstElemInit = 0;
            int secondElemInit = 0;

            const int firstPos = rand() % list.getSize();
            const int secondPos = rand() % list.getSize();

            list.getLogic(firstPos, &firstElemInit);
            list.getLogic(secondPos, &secondElemInit);

            list.swapLogic(firstPos, secondPos);

            list.getLogic(firstPos, &firstElem);
            list.getLogic(secondPos, &secondElem);

            EXPECT_EQ(firstElem, secondElemInit);
            EXPECT_EQ(firstElemInit, secondElem);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }

        list.deOptimize();

        for (size_t i = 0; i < testSize; i++) {
            int firstElem = 0;
            int secondElem = 0;
            int firstElemInit = 0;
            int secondElemInit = 0;

            const int firstPos = rand() % list.getSize();
            const int secondPos = rand() % list.getSize();

            list.getLogic(firstPos, &firstElemInit);
            list.getLogic(secondPos, &secondElemInit);

            list.swapLogic(firstPos, secondPos);

            list.getLogic(firstPos, &firstElem);
            list.getLogic(secondPos, &secondElem);

            EXPECT_EQ(firstElem, secondElemInit);
            EXPECT_EQ(firstElemInit, secondElem);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, getters) {
    FILE* logFile = fopen("/dev/null", "w");
    SwiftyList<int> list(10, 2, logFile, false);
    EXPECT_TRUE(list.getParams()->getLogFile() == logFile );
    EXPECT_TRUE(list.getParams()->getChecks() == false );
    EXPECT_TRUE(list.getParams()->getVerbose() == 2 );
}

TEST(SwiftyListTests, optimize) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, NULL, false);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            if (i % 2 == 1)
                list.pushBack(i);
            else
                list.pushFront(i);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}