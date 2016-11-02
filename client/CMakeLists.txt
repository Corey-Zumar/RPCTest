cmake_minimum_required(VERSION 3.6)
project(Test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp RPCClient.cpp RPCClient.h ModelContainer.cpp ModelContainer.h RPCServer.cpp RPCServer.h RPCUtil.cpp RPCUtil.h ModelConfigStore.cpp ModelConfigStore.h RPCConnection.cpp RPCConnection.h)
add_executable(Test ${SOURCE_FILES})

include_directories(
        /Users/Corey/Documents/AMP/cppzmq
)

find_library(ZMQ_LIB NAMES zmq PATHS
        /Users/Corey/Documents/AMP/zeromq-4.1.5/src
        /Users/Corey/Documents/AMP/zeromq-4.1.5
)

target_link_libraries(Test ${ZMQ_LIB})