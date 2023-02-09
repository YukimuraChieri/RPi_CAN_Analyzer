cmake_minimum_required(VERSION 3.15.0)
project(CAN_Analyzer)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(SOURCE_DIR src/)

aux_source_directory(${SOURCE_DIR} SOURCE_LIST)

message("source files:" ${SOURCE_LIST})

add_executable(CAN_Analyzer ${SOURCE_LIST})

target_compile_options(CAN_Analyzer PUBLIC -Wall -Werror=sign-compare)


