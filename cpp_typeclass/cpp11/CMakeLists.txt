cmake_minimum_required(VERSION 2.4)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wextra -std=c++11")

include_directories(.)

add_executable(typeclass-example typeclass-example.cpp)
