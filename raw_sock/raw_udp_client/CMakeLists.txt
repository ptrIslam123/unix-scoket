cmake_minimum_required(VERSION 3.20)

project(raw_udp_client)
set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    net_interface_info
    parser
    package_headers
)