# CMakeLists files in this project can
cmake_minimum_required (VERSION 2.8.11)

project(cpp_can_tunnel)

add_subdirectory( lib )

find_package(Threads REQUIRED)

add_executable (
    cpp_can_tunnel
    app/src/cpp_can_tunnel.cpp
    )

target_include_directories ( cpp_can_tunnel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/app/include )

target_link_libraries( cpp_can_tunnel socketcan_wrapper tcp_server_client_wrapper "${CMAKE_THREAD_LIBS_INIT}" )
