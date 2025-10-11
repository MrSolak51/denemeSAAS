cmake_minimum_required(VERSION 3.10)
project(hello_render)

find_package(Crow)


set(CMAKE_CXX_STANDARD 17)
add_executable(hello_render main.cpp)
