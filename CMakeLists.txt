cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

project(Raytracing)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(Raytracing src/main.cpp)
