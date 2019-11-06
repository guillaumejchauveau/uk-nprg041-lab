cmake_minimum_required(VERSION 3.10)
project(Lab)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
add_executable(Lab
        Lab.cpp Str.cpp)
