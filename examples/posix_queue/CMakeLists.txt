cmake_minimum_required(VERSION 3.0)

project( example_posix_queue )

find_library( EVENT event )
find_library( PTHREAD pthread )
find_library( RT rt )

add_executable(
    example_posix_queue
    example_posix_queue.cpp
    example_loop.cpp
)


target_link_libraries(
    example_posix_queue
    ${EVENT}
    ${PTHREAD}
    eventloop
    ${RT}
)
