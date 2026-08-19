cmake_minimum_required(VERSION 4.3.2)

project(redis_clone VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(aof_logger STATIC logs/aof_logger.cpp)
target_compile_features(aof_logger PUBLIC cxx_std_17)

add_executable(redisdb main.cpp utils/utils.cpp engine/engine.cpp servers/cliserver/cliserver.cpp servers/tcpserver/tcpserver.cpp)
target_link_libraries(redisdb PRIVATE aof_logger)