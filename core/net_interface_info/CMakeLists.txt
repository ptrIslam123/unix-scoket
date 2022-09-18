cmake_minimum_required(VERSION 3.20)

project(net_interface_info)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} STATIC interface_info.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})