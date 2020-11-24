cmake_minimum_required(VERSION 3.10)

project(BV_Praktikum)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_STANDARD_REQUIRED True)



include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(BV_Praktikum Quelle.cpp)

target_link_libraries(BV_Praktikum ${OpenCV_LIBS})