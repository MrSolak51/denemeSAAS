cmake_minimum_required(VERSION 3.10)
project(metehan_turgut)

include_directories(${CROW_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)
add_executable(server main.cpp)
