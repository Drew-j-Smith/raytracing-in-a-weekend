cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

project(Raytracing)

include_directories(SOURCE_DIR)

add_executable(Raytracing src/main.cpp)