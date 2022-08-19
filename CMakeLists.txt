# CMakeLists files in this project can
cmake_minimum_required (VERSION 2.8.11)

project(cpp_can_tunnel)

add_compile_options(-std=c++11)

add_subdirectory( lib )

find_package(Threads REQUIRED)

add_library(
    cpp_can_tunnel
    app/src/cpp_can_tunnel.cpp
    )

target_include_directories ( cpp_can_tunnel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/app/include )

target_link_libraries( cpp_can_tunnel socketcan_wrapper tcp_server_client_wrapper "${CMAKE_THREAD_LIBS_INIT}" )

add_executable(
    can_tunnel
    app/src/cpp_can_tunnel_app.cpp
)

target_link_libraries(
    can_tunnel
    cpp_can_tunnel
)

add_executable(
    can_tunnel_test
    test/test.cpp
)

target_link_libraries(
    can_tunnel_test
    cpp_can_tunnel
)

install(
    TARGETS can_tunnel
    DESTINATION bin
)
