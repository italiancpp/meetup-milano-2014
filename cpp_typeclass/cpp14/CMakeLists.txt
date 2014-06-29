cmake_minimum_required(VERSION 2.4)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wextra -std=c++1y")

include_directories(hdr)

add_executable(example example.cpp)
