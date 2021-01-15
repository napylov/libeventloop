#ifndef TEST_SERVER_LOOP_BASE_H
#define TEST_SERVER_LOOP_BASE_H

#include <atomic>
#include <gtest/gtest.h>
#include "eventloop/server_loop_base.h"

using namespace eventloop;

class test_server_loop_base_run;


class test_server_loop_base : public server_loop_base
{
public:
    std::atomic_int     runned_threads;
    bool                on_accept_called;
    bool                on_client_called;

    friend class test_server_loop_base_run;

public:
    test_server_loop_base();

    bool test_on_accept();
    bool test_run_threads();
    bool test_call_callback();
    bool test_make_callback_accept_info();

    virtual void on_client( evutil_socket_t fd, short what ) override;
    virtual void process_thread_fn() override;
    virtual void on_accept
    (
            evutil_socket_t         fd,
            const struct sockaddr   *addr,
            int                     sock_len
    );

};


class test_server_loop_base_run : public ::testing::Test
{
public:
    std::unique_ptr<test_server_loop_base>  tested_class;

public:
    test_server_loop_base_run() = default;

    virtual void SetUp() override;
    virtual void TearDown() override;
};


#endif // TEST_SERVER_LOOP_BASE_H
