cmake_minimum_required(VERSION 3.8)
project(ecse_427)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

set(SOURCE_FILES
        asg2/air_taxi_sim_skeleton.c)

add_executable(ecse_427  ${SOURCE_FILES})