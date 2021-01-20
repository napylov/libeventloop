cmake_minimum_required(VERSION 3.0)

project(eventloop)

enable_testing()

set( SRC_DIR ${PROJECT_SOURCE_DIR}/src )

set( DOC_DIR ${PROJECT_SOURCE_DIR}/doc )
set( INC_DIR ${PROJECT_SOURCE_DIR}/include )

set( TEST_SUBDIR test )
set( TEST_DIR ${PROJECT_SOURCE_DIR}/${TEST_SUBDIR} )

set( CMAKE_CXX_FLAGS "-Wall -g -std=c++17" )

find_library( EVENT event )
find_library( GTEST gtest )
find_library( PTHREAD pthread )

include_directories( ${INC_DIR} )

set(
    EVENTLOOP_SRC
    ${SRC_DIR}/loop.cpp
    ${SRC_DIR}/server_loop_base.cpp
    ${SRC_DIR}/fd_factory.cpp
)

add_library(
    eventloop
    SHARED
    ${EVENTLOOP_SRC}
)

target_link_libraries(
    eventloop
#    ${CPPSTD}
    ${EVENT}
    ${GTEST}
    ${PTHREAD}
)

add_subdirectory( test ${TEST_SUBDIR} )
