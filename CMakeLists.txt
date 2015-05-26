cmake_minimum_required(VERSION 3.0)
project(iojxd)

option(BUILDEBUG "enable -g" OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # LuaJIT needs this
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    message(FATAL_ERROR "Windows is not supported by iojxd!")
    return()
endif()

if (BUILDEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

add_executable(iojxd src/iojxd.cxx src/child_termcb.hxx src/child_termcb.cxx
    src/luafound.cxx src/luafound.hxx src/foundation.cxx src/foundation.hxx
    src/util.hxx src/debug.hxx src/context.hxx src/common.hxx src/context.cxx
    src/lbext.hxx src/util.cxx src/sock.cxx src/sock.hxx src/lua_inc.hxx
    src/timer.cxx src/timer.hxx src/childprocess.hxx src/childprocess.cxx
    src/luafoundx.cxx src/luafoundx.hxx src/sandbox.cxx src/sandbox.hxx)

target_link_libraries(iojxd luajit-5.1)
target_link_libraries(iojxd ev)

include_directories(SYSTEM ./src)

add_executable(iojxd_test src/util.cxx test/test_cycqueue.cxx)
add_executable(testrlimit test/test_reslimit.cxx)
add_executable(testloop test/looptest.cxx)
