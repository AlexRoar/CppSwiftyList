
cmake_minimum_required(VERSION 3.4)
project(SwiftyList CXX)

ADD_SUBDIRECTORY(googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(SL_SOURCES
        SwiftyList/SwiftyList.cpp
        SwiftyList/Logging/GraphVizDumper.cpp
        )

include_directories(
        SwiftyList
        SwiftyList/Logging
)

add_library(SwiftyList SHARED
        ${SL_SOURCES}
        )

add_executable(SwiftyListExample
        Examples/main.cpp
        )

add_executable(SwiftyListTests
        Examples/SLTests.cpp
        )
target_link_libraries(SwiftyListTests gtest gtest_main SwiftyList)
target_link_libraries(SwiftyListExample SwiftyList)
add_test(SwiftyListTests SwiftyListTests)


