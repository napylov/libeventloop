#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <future>

#include "test_server_loop.h"
#include "eventloop/loop_obj_factory.h"
#include "eventloop/fd_factory.h"


test_server_loop::test_server_loop()
: server_loop(), fd_ipv4( -1 ), fd_ipv6( -1 )
{
}


test_server_loop::~test_server_loop()
{
    if ( fd_ipv4 >= 0 )
        close( fd_ipv4 );
    if ( fd_ipv6 >= 0 )
        close( fd_ipv6 );
}


void test_server_loop::process_event( event_queue_item &&item )
{
    static size_t BUF_SIZE = 1023;
    char buf[ BUF_SIZE + 1 ] = { 0 };

    ssize_t has_been_read = read( item.fd, buf, BUF_SIZE );
    if ( has_been_read > 0 )
    {
        try
        {
            std::string tmp( buf, has_been_read );
            output_strings.emplace_back( std::move( tmp ) );
        }
        catch(...)
        {
        }
    }
}


bool test_server_loop::test_connect_ipv4()
{
    fd_ipv4 = fd_factory::connect( "localhost", DEFAULT_PORT, AF_INET );

    if ( fd_ipv4 != fd_factory::INVALID_FD )
        std::cout << "Connected successfull.\n";
    else
        std::cout << "Connected failed.\n";

    return true;
}


bool test_server_loop::test_connect_ipv6()
{
    fd_ipv6 = fd_factory::connect( "localhost", DEFAULT_PORT, AF_INET6 );

    if ( fd_ipv6 != fd_factory::INVALID_FD )
        std::cout << "Connected successfull IPv6.\n";
    else
        std::cout << "Connected failed IPv6.\n";

    return true;

}


bool test_server_loop::test_on_client()
{
    return true;
}



class test_server_loop_run : public ::testing::Test
{
public:
    std::unique_ptr<test_server_loop>    tested_class;
public:
    test_server_loop_run() = default;

    virtual void SetUp() override
    {
        tested_class = loop_obj_factory::make<test_server_loop>( false );
    }


    virtual void TearDown() override
    {
        tested_class.reset();
    }
};


TEST_F( test_server_loop_run, fn_connect_ipv4 )
{
    ASSERT_TRUE( tested_class.operator bool() );

    bool success = false;

    std::async
    (
        std::launch::async,
        [&] ()  {
                    sleep( 1 );
                    success = tested_class->test_connect_ipv4();
                    ASSERT_TRUE( tested_class->stop() );
                    sleep( 1 );
                }
    );

    if ( tested_class )
    {
        ASSERT_TRUE( tested_class->run() == loop::run_result::OK );
    }

    ASSERT_TRUE( success );
}



TEST_F( test_server_loop_run, fn_connect_ipv6 )
{
    ASSERT_TRUE( tested_class.operator bool() );

    bool success = false;

    std::async
    (
        std::launch::async,
        [&] ()  {
                    sleep( 1 );
                    success = tested_class->test_connect_ipv6();
                    ASSERT_TRUE( tested_class->stop() );
                    sleep( 1 );
                }
    );

    if ( tested_class )
    {
        ASSERT_TRUE( tested_class->run() == loop::run_result::OK );
    }

    ASSERT_TRUE( success );
}
