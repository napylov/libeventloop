#include <event2/event.h>
#include "test_server_loop_base.h"


void test_server_loop_base_run::SetUp()
{
    tested_class.reset( new test_server_loop_base );
    tested_class->make_base();
}


void test_server_loop_base_run::TearDown()
{
    tested_class.reset();
}


test_server_loop_base::test_server_loop_base()
    : runned_threads( 0 ), on_accept_called( false ), on_client_called( false )
{
}


bool test_server_loop_base::test_on_accept()
{
    static const int TEST_FD = 0;
    static const sockaddr TEST_ADDR = {0};
    static const int TEST_ADDR_LEN = sizeof( TEST_ADDR );

    on_accept( TEST_FD, &TEST_ADDR, TEST_ADDR_LEN );

    auto it = fd_events.find( TEST_FD );

    if ( it == fd_events.end() )
        return false;

    bool result =
        event_get_base( it->second.get() ) == base.get() &&
        event_get_fd( it->second.get() ) == TEST_FD
    ;

    fd_events.erase( it );

    return result;
}


bool test_server_loop_base::test_run_threads()
{
    run_threads( DEFAULT_THREADS_COUNT );
    sleep( 1 );
    return runned_threads.load() == DEFAULT_THREADS_COUNT;
}


bool test_server_loop_base::test_call_callback()
{
    std::unique_ptr<callback_accept_info> info( make_callback_accept_info() );
    if ( !info )
        return false;

    on_accept_called = false; // changed in call_callback()

    static const int TEST_FD = 0;
    static sockaddr TEST_ADDR = {0};
    static const int TEST_ADDR_LEN = sizeof( TEST_ADDR );

    call_callback( listener.get(), TEST_FD, &TEST_ADDR, TEST_ADDR_LEN, info.get() );

    return on_accept_called;
}


bool test_server_loop_base::test_make_callback_accept_info()
{
    static const int TEST_FD = 0;
    static sockaddr TEST_ADDR = {0};
    static const int TEST_ADDR_LEN = sizeof( TEST_ADDR );

    on_accept( TEST_FD, &TEST_ADDR, TEST_ADDR_LEN );

    std::unique_ptr<callback_accept_info> info( make_callback_accept_info() );
    if ( !info )
        return false;

    info->fn( TEST_FD, &TEST_ADDR, TEST_ADDR_LEN );
    auto it = fd_events.find( TEST_FD );
    bool result = it != fd_events.end();

    if ( result )
        fd_events.erase( it );

    return result;
}


void test_server_loop_base::on_client( evutil_socket_t fd, short what, const std::nullptr_t & )
{
    on_client_called = true;
}


void test_server_loop_base::process_thread_fn()
{
    runned_threads++;

    while ( work_flag )
        sleep( 1 );
}


void test_server_loop_base::on_accept
(
        evutil_socket_t         fd,
        const struct sockaddr   *addr,
        int                     sock_len
)
{
    on_accept_called = true;
    server_loop_base::on_accept( fd, addr, sock_len );
}


TEST_F( test_server_loop_base_run, fn_run_threads )
{
    ASSERT_TRUE( tested_class->test_run_threads() );
}


TEST_F( test_server_loop_base_run, fn_on_accept )
{
    ASSERT_TRUE( tested_class->test_on_accept() );
}


TEST_F( test_server_loop_base_run, fn_make_callback_accept_info )
{
    ASSERT_TRUE( tested_class->test_make_callback_accept_info() );
}

TEST_F( test_server_loop_base_run, fn_call_callback )
{
    ASSERT_TRUE( tested_class->test_call_callback() );
}

