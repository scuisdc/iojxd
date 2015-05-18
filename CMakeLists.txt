cmake_minimum_required(VERSION 3.0)
project(iojxd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000")

set(SOURCE_FILES)
add_executable(iojxd ${SOURCE_FILES} iojxd.cxx child_termcb.hxx child_termcb.cxx luafound.cxx luafound.hxx foundation.cxx foundation.hxx util.hxx debug.hxx context.hxx common.hxx context.cxx lbext.hxx)

target_link_libraries(iojxd luajit)
target_link_libraries(iojxd event)
