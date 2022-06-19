cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(Raytracing)

find_package(OpenCL REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}  src/main.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE CL_HPP_TARGET_OPENCL_VERSION=210)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
target_compile_definitions(${PROJECT_NAME} PRIVATE)


add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/src/main.cl
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/main.cl)