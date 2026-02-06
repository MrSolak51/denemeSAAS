cmake_minimum_required(VERSION 3.10)
project(metehan_turgut)


find_package(OpenCV REQUIRED)

include_directories(
        ${CROW_INCLUDE_DIR}
        ${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)
add_executable(server main.cpp
        ImageConverter.cpp
        ImageConverter.h
        DocumentConverter.cpp
        DocumentConverter.h)


target_link_libraries(server ${OpenCV_LIBRARIES})
