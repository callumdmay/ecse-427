cmake_minimum_required(VERSION 3.8)
project(ecse_427)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

set(SOURCE_FILES
        asg3/260564523_part2.c)

add_executable(ecse_427  ${SOURCE_FILES})