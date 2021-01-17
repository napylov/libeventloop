#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <future>

#include "test_server_loop.h"
#include "eventloop/loop_obj_factory.h"


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
    fd_ipv4 = socket( AF_INET, SOCK_STREAM, 0 );
    if ( fd_ipv4 < 0 )
        return false;

    struct sockaddr_in serv_addr = { 0 };

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons( DEFAULT_PORT );

    if( inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0 )
    {
        perror( "Error: inet_pton error occured\n" );
        return false;
    }

    if( connect(fd_ipv4, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror( "Error : Connect Failed\n");
        return false;
    }

    std::cout << "Connected successfull.\n";

    return true;
}


bool test_server_loop::test_connect_ipv6()
{
    fd_ipv6 = socket( AF_INET6, SOCK_STREAM, 0 );
    if ( fd_ipv6 < 0 )
        return false;

    struct sockaddr_in6 serv_addr = { 0 };

    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons( DEFAULT_PORT );

    if( inet_pton(AF_INET6, "::1", &serv_addr.sin6_addr) <= 0 )
    {
        perror( "Error: inet_pton error occured (IPv6)\n" );
        return false;
    }

    if( connect(fd_ipv6, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror( "Error : Connect Failed (IPv6)\n");
        return false;
    }

    std::cout << "Connected successfull (IPv6).\n";

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
