cmake_minimum_required(VERSION 3.20)

project(parser)
set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} parser_net_package.cpp)

target_link_libraries(${PROJECT_NAME} INTERFACE types)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC ../)