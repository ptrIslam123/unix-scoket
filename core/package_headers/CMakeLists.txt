cmake_minimum_required(VERSION 3.20)

project(package_headers)
set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} STATIC
    package_headers.cpp
)

target_link_libraries(${PROJECT_NAME} INTERFACE types)

target_include_directories(${PROJECT_NAME} PUBLIC ../)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})