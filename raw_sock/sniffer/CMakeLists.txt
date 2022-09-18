cmake_minimum_required(VERSION 3.23)
project(sniffer)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    net_interface_info
    parser
)
