cmake_minimum_required(VERSION 3.10)
project(CPP_Lab)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
