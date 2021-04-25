cmake_minimum_required(VERSION 3.0)

project( example_server )

find_library( EVENT event )
find_library( PTHREAD pthread )
find_library( RT rt )

add_executable(
    example_server
    example_server.cpp
    example_server_loop.cpp
    example_custom_data.cpp
)


target_link_libraries(
    example_server
    ${EVENT}
    ${PTHREAD}
    eventloop
    ${RT}
)


set_property(TARGET example_server PROPERTY CXX_STANDARD 17)
