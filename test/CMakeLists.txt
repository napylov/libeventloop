cmake_minimum_required(VERSION 3.0)

project(eventloop_test)

find_package( gtest )

find_library( EVENT event )
find_library( GTEST gtest )
find_library( PTHREAD pthread )


set( TEST_SRC_DIR ${PROJECT_SOURCE_DIR}/src )

set( TEST_UTILITY eventloop_test )

set(
    TEST_SRC

    ${EVENTLOOP_SRC}

    ${TEST_SRC_DIR}/test_loop.cpp
    ${TEST_SRC_DIR}/test_main.cpp
    ${TEST_SRC_DIR}/test_event_queue.cpp
)


add_executable(
    ${TEST_UTILITY}
    ${TEST_SRC}
)


target_link_libraries(
    ${TEST_UTILITY}
    ${EVENT}
    ${GTEST}
    ${PTHREAD}
)


add_test(
    NAME    ${TEST_UTILITY}
    COMMAND ${TEST_UTILITY}
)

