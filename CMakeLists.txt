cmake_minimum_required(VERSION 4.3.2)

project(redis_clone VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(redisdb main.cpp utils/utils.cpp engine/engine.cpp servers/cliserver/cliserver.cpp servers/tcpserver/tcpserver.cpp)