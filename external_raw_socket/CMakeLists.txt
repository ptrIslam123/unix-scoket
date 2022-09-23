cmake_minimum_required(VERSION 3.20)

project(extern_raw_socket)

add_executable(external_sniffer packet_sniff_raw_subodh.cpp)

add_executable(external_sender send_packet_raw_subodh.cpp)

add_executable(external_sender2 sender_package_raw_socket_2.cpp)