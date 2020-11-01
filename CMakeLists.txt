
cmake_minimum_required (VERSION 3.4)
project(SwiftyList CXX)

ADD_SUBDIRECTORY (googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set( SL_SOURCES
        SwiftyList/SwiftyList.cpp
        SwiftyList/Logging/GraphVizDumper.cpp
)

include_directories(
        SwiftyList
        SwiftyList/Logging
)

add_executable(SwiftyList
        ${SL_SOURCES}
        Examples/main.cpp
)

add_executable(SwiftyListTests
        ${SL_SOURCES}
        Examples/SLTests.cpp )
target_link_libraries(SwiftyListTests gtest gtest_main)
add_test( SwiftyListTests SwiftyListTests )


