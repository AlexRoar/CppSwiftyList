//
// Created by Александр Дремов on 01.11.2020.
//

#include "gtest/gtest.h"
#include <cstdlib>
#include "SwiftyList.hpp"

#define CAPACITY_RANGE 500

TEST(SwiftyListTests, checkUp) {
    SwiftyList<int> list(0, 0, nullptr, false);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    list.pushBack(1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 100; i++) {
        list.pushFront(i);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 100; i++) {
        list.popFront(nullptr);
    }
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}


TEST(SwiftyListTests, pushBack) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, nullptr, false);
        for (size_t i = 0; i < testSize; i++) {
            list.pushBack(i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        EXPECT_EQ(list.isOptimized(), true);
        for (size_t i = 0; i < testSize; i++) {
            int tmp = 0;
            size_t it = list.logicToPhysic(i);
            list.get(it, &tmp);
            EXPECT_EQ(tmp, i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, pushFront) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, nullptr, false);
        for (size_t i = 0; i < testSize; i++) {
            EXPECT_TRUE(list.pushFront(i) == LIST_OP_OK);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            int tmp = 0;
            size_t it = list.logicToPhysic(i);
            EXPECT_TRUE(list.get(it, &tmp) == LIST_OP_OK);
            EXPECT_EQ(tmp, testSize - i - 1);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, popFront) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, nullptr, false);
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
        SwiftyList<int> list(0, 0, nullptr, false);
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

TEST(SwiftyListTests, setGetPhysic) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, nullptr, false);
        for (size_t i = 0; i < testSize; i++) {
            EXPECT_TRUE(list.pushFront(i) == LIST_OP_OK);
        }
        list.deOptimize();
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            int value = rand();
            EXPECT_TRUE(list.set(i + 1, value) == LIST_OP_OK);
            int gValue = 0;
            EXPECT_TRUE(list.get(i + 1, &gValue) == LIST_OP_OK);
            EXPECT_EQ(value, gValue);
        }

        list.optimize();

        for (size_t i = 0; i < testSize; i++) {
            int value = rand();
            EXPECT_TRUE(list.set(i + 1, value) == LIST_OP_OK);
            int gValue = 0;
            EXPECT_TRUE(list.get(i + 1, &gValue) == LIST_OP_OK);
            EXPECT_EQ(value, gValue);
        }

        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, searchLinear) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, nullptr, false);
        for (size_t i = 0; i < testSize; i++) {
            list.pushFront(2 * i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            size_t pos = 0;
            list.searchLinear(&pos, 2 * i);
            EXPECT_EQ(pos, i);
        }
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, relloacations) {
    SwiftyList<int> list(10, 0, nullptr, false);
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

TEST(SwiftyListTests, removePhysic) {
    SwiftyList<int> list(0, 0, nullptr, false);
    for (size_t i = 0; i < 10; i++) {
        list.pushBack(i);
    }
    // 0 1 2 3 4 5 6 7 8 9
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    list.remove(1);

    // 1 2 3 4 5 6 7 8 9
    int val = 0;
    size_t it = list.logicToPhysic(0);
    list.get(it, &val);

    EXPECT_EQ(val, 1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    // 1 2 3 5 6 7 8 9
    list.remove(5);
    it = list.logicToPhysic(3);
    list.get(it, &val);

    EXPECT_EQ(val, 5);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, sideCases) {
    SwiftyList<int> list(0, 0, nullptr, false);
    list.pushBack(1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    int val = 0;
    size_t it = list.logicToPhysic(0);
    list.get(it, &val);
    EXPECT_TRUE(list.get(it, &val) == LIST_OP_OK);
    EXPECT_EQ(val, 1);

    list.remove(it);
    it = list.logicToPhysic(0);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    EXPECT_TRUE(list.get(it, &val) != LIST_OP_OK);

    EXPECT_TRUE(list.remove(it) != LIST_OP_OK);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);

    list.pushFront(1);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, clear) {
    SwiftyList<int> list(0, 0, nullptr, false);
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    for (size_t i = 0; i < 10; i++) {
        list.pushBack(i);
    }

    list.clear();
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    EXPECT_TRUE(list.getSize() == 0);
}

TEST(SwiftyListTests, empty) {
    SwiftyList<int> list(0, 0, nullptr, false);
    EXPECT_EQ(0, list.getSize());
    EXPECT_TRUE(list.isEmpty());
    EXPECT_TRUE(list.isOptimized());
    EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
}

TEST(SwiftyListTests, pushPop) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, nullptr, false);
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
        SwiftyList<int> list(0, 0, nullptr, false);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            if (i % 2 == 0)
                list.pushBack(i);
            else
                list.pushFront(i);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }
        for (size_t i = 0; i < testSize; i++) {
            list.remove(rand() % list.getSize());
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }
    }
}

TEST(SwiftyListTests, capacity) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, nullptr, false);
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
        SwiftyList<int> list(0, 0, nullptr, false);
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
        SwiftyList<int> list(0, 0, nullptr, false);
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

TEST(SwiftyListTests, getters) {
    FILE* logFile = fopen("/dev/null", "w");
    SwiftyList<int> list(10, 2, logFile, false);
    EXPECT_TRUE(list.getParams()->getLogFile() == logFile );
    EXPECT_TRUE(list.getParams()->getChecks() == false );
    EXPECT_TRUE(list.getParams()->getVerbose() == 2 );
}

TEST(SwiftyListTests, optimize) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, nullptr, false);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            if (i % 2 == 1)
                list.pushBack(i);
            else
                list.pushFront(i);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }
        list.optimize();
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
    }
}

TEST(SwiftyListTests, iterators) {
    for (size_t testSize = 0; testSize < CAPACITY_RANGE; testSize++) {
        SwiftyList<int> list(0, 0, nullptr, false);
        EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        for (size_t i = 0; i < testSize; i++) {
            if (i % 2 == 1)
                list.pushBack(i);
            else
                list.pushFront(i);
            EXPECT_TRUE(list.checkUp() == LIST_OP_OK);
        }

        size_t val = 0;
        size_t iterator = 0;
        list.nextIterator(&iterator);
        for (; iterator != 0; list.nextIterator(&iterator)) {
            int tmp = 0;
            EXPECT_TRUE(list.get(iterator, &tmp) == LIST_OP_OK);
            EXPECT_EQ(tmp, val);
            val++;
        }

        val = 0;
        iterator = 0;
        list.prevIterator(&iterator);
        for (; iterator != 0; list.prevIterator(&iterator)) {
            int tmp = 0;
            EXPECT_TRUE(list.get(iterator, &tmp) == LIST_OP_OK);
            EXPECT_EQ(tmp,testSize - val - 1);
            val++;
        }
    }
}