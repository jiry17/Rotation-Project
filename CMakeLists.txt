cmake_minimum_required(VERSION 3.13)
project(ISTool)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -Ofast")
add_definitions(-DDEBUG)

INCLUDE(FindPkgConfig)
include_directories(include)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    #jsoncpp
    find_package(Jsoncpp)
    include_directories(${Jsoncpp_INCLUDE_DIR})

    set(THIRDPARTY_LIBS glog gflags ${Jsoncpp_LIBRARY} )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    #jsoncpp
    set(Jsoncpp_LIBRARY /usr/local/lib/libjsoncpp.dylib)
    include_directories(/usr/local/include)

    #glog
    include_directories(/usr/local/opt/glog/include/)
    set(GLOG_FILE /usr/local/opt/glog/lib/libglog.dylib)

    # libs
    set(THIRDPARTY_LIBS ${Jsoncpp_LIBRARY} ${GLOG_FILE})
else()
    message(FATAL_ERROR "Unexceptional system ${CMAKE_SYSTEM_NAME}")
endif()

# subdic
add_subdirectory(basic)
add_subdirectory(sygus)

set(ALL_LIBS ${SyGuS_LIBS} basic_lib)

add_subdirectory(tasks)