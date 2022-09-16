cmake_minimum_required(VERSION 3.23)
project(sniffer)

set(CMAKE_CXX_STANDARD 20)

add_executable(raw_sock
    main.cpp
    interface_attr.cpp
    parser_net_package.cpp
)
