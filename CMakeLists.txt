project(eventloop)

set( SRC_DIR ${PROJECT_SOURCE_DIR}/src )
set( DOC_DIR ${PROJECT_SOURCE_DIR}/doc )
set( INC_DIR ${PROJECT_SOURCE_DIR}/include )

add_library(
    eventloop
    ${SRC_DIR}/loop.cpp
)
