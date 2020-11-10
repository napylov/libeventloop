cmake_minimum_required(VERSION 3.0)

project(eventloop)

set( SRC_DIR ${PROJECT_SOURCE_DIR}/src )
set( DOC_DIR ${PROJECT_SOURCE_DIR}/doc )
set( INC_DIR ${PROJECT_SOURCE_DIR}/include )

set( CMAKE_CXX_FLAGS "-Wall -std=c++17" )

find_library( stdc++ event )

include_directories( ${INC_DIR} )

set(
    SRC
    ${SRC_DIR}/loop.cpp
)

add_library(
    eventloop
    SHARED
    ${SRC}
)
