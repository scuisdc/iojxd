cmake_minimum_required(VERSION 3.0)
project(iojxd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

set(SOURCE_FILES)
add_executable(iojxd ${SOURCE_FILES} iojxd.cxx)

target_link_libraries(iojxd lua)
target_link_libraries(iojxd event)
