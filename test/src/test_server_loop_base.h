#ifndef TEST_SERVER_LOOP_BASE_H
#define TEST_SERVER_LOOP_BASE_H

#include <atomic>
#include <gtest/gtest.h>
#include "eventloop/server_loop_base.h"

using namespace eventloop;

class test_server_loop_base : public server_loop_base, public ::testing::Test
{
public:
    std::atomic_int    runned_threads;
    bool               on_accept_called;

public:
    test_server_loop_base();

    bool test_on_accept();
    bool test_run_threads();
    bool test_call_callback();
    bool test_make_callback_accept_info();

    virtual void on_client( evutil_socket_t fd, short what ) override;
    virtual void process_thread_fn( std::atomic_bool &work_flag ) override;
    virtual void on_accept
    (
            evutil_socket_t         fd,
            const struct sockaddr   *addr,
            int                     sock_len
    );

};

#endif // TEST_SERVER_LOOP_BASE_H
