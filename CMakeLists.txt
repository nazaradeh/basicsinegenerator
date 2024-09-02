cmake_minimum_required(VERSION 3.10)
project(basicsinegenerator)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)

add_executable(basicsinegenerator main.cpp)

target_link_libraries(basicsinegenerator PRIVATE Winmm)
