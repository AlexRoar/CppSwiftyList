
cmake_minimum_required(VERSION 3.4)
project(SwiftyList CXX)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -Wextra -g")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O9")

ADD_SUBDIRECTORY(googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(SL_SOURCES
        SwiftyList/SwiftyList.cpp
        )

include_directories(
        SwiftyList
)

add_library(SwiftyList SHARED
        ${SL_SOURCES}
        )

add_executable(SwiftyListExample
        Examples/main.cpp
        )

add_executable(SwiftyListTests
        Examples/SLTests.cpp
        ${SL_SOURCES}
        )
target_link_libraries(SwiftyListTests gtest gtest_main)
target_link_libraries(SwiftyListExample SwiftyList)
add_test(SwiftyListTests SwiftyListTests)


