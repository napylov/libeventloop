cmake_minimum_required(VERSION 3.0)

project(eventloop)

enable_testing()

set( SRC_DIR ${PROJECT_SOURCE_DIR}/src )

set( DOC_DIR ${PROJECT_SOURCE_DIR}/doc )
set( INC_DIR ${PROJECT_SOURCE_DIR}/include )

set( TEST_SUBDIR test )
set( TEST_DIR ${PROJECT_SOURCE_DIR}/${TEST_SUBDIR} )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Debug )
endif()


if ( CMAKE_BUILD_TYPE STREQUAL "Release" )
    set( BUILD_FLAGS "-O3" )
else()
    set( BUILD_FLAGS "-g -O0 -DDEBUG_BUILD" )
endif()


set( CMAKE_CXX_FLAGS "-Wall ${BUILD_FLAGS} -std=c++17" )

find_library( EVENT event )
find_library( GTEST gtest )
find_library( PTHREAD pthread )

include_directories( ${INC_DIR} )

set(
    EVENTLOOP_SRC
    ${SRC_DIR}/fd_factory.cpp
    ${SRC_DIR}/log.cpp
)

add_library(
    eventloop
    SHARED
    ${EVENTLOOP_SRC}
)

target_link_libraries(
    eventloop
    ${EVENT}
    ${GTEST}
    ${PTHREAD}
)

add_subdirectory( test ${TEST_SUBDIR} )
add_subdirectory( examples )


install(
    TARGETS eventloop
    LIBRARY DESTINATION lib
)

install(
    DIRECTORY ${INC_DIR}/eventloop
    PUBLIC_HEADER DESTINATION include
)
