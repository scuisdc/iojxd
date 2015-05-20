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

set(SOURCE_FILES)
add_executable(iojxd ${SOURCE_FILES} src/iojxd.cxx src/child_termcb.hxx src/child_termcb.cxx
src/luafound.cxx src/luafound.hxx src/foundation.cxx src/foundation.hxx src/util.hxx src/debug.hxx src/context.hxx src/common.hxx src/context.cxx src/lbext.hxx src/util.cxx src/sock.cxx src/sock.hxx src/lua_inc.hxx src/timer.cxx src/timer.hxx src/childprocess.hxx src/childprocess.cxx)

target_link_libraries(iojxd luajit)
target_link_libraries(iojxd ev)

include_directories(SYSTEM ./src)
add_executable(iojxd_test src/util.cxx test/test_cycqueue.cxx)
