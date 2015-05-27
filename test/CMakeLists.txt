cmake_minimum_required(VERSION 3.0)
project(iojxd_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

add_executable(iojxd_test ../src/util.cxx test_cycqueue.cxx)
add_executable(testrlimit test_reslimit.cxx)
add_executable(testloop looptest.cxx)
add_executable(testsort test_sort.cxx)
