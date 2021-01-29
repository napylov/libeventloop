#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <future>
#include <array>

#include "test_server_loop.h"
#include "eventloop/loop_obj_factory.h"
#include "eventloop/fd_factory.h"


test_server_loop::test_server_loop()
: server_loop(), signal_event(), fd_ipv4( -1 ), fd_ipv6( -1 )
{
}


test_server_loop::~test_server_loop()
{
    if ( fd_ipv4 >= 0 )
        close( fd_ipv4 );
    if ( fd_ipv6 >= 0 )
        close( fd_ipv6 );
    ::close( event_get_fd( signal_event.get() ) );
}


void test_server_loop::process_event( event_queue_item<tsl_custom> &&item )
{
    if ( item.fd < 0 )
        return;

    static size_t BUF_SIZE = 1023;
    char buf[ BUF_SIZE + 1 ] = { 0 };

    std::cout << __PRETTY_FUNCTION__ << "\n";
    std::cout << "fd [" << item.fd << "]\n";

    ssize_t has_been_read = read( item.fd, buf, BUF_SIZE );
    std::cout << "read " << has_been_read << " bytes. errno " << errno << "\n";
    if ( has_been_read > 0 )
    {
        try
        {
            std::string tmp( buf, has_been_read );
            output_strings.emplace_back( std::move( tmp ) );
        }
        catch(std::exception &e)
        {
            std::cout << "Exception: " << e.what() << "\n";
        }
        catch(...)
        {
            std::cout << "Exception\n";
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


bool test_server_loop::init_custom_events()
{
    signal_event =
        make_event
        (
            fd_factory::create_signal_fd( std::array<int,1>{ SIGINT } ),
            EV_READ,
            std::bind(
                        &test_server_loop::on_signal,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2
                     )
        )
    ;

    return signal_event.operator bool();
}


void test_server_loop::on_signal( int fd, int what )
{
    stop();
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


    int send_string( const std::string &str )
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
        int fd = fd_factory::connect( "localhost", server_loop_base<tsl_custom>::DEFAULT_PORT, AF_INET );
        if ( fd < 0 )
            return false;

        std::cout << "str [" << str << "]\n";

        bool result =
                ::send( fd, str.c_str(), str.length(), 0 ) ==
                (ssize_t)str.length()
        ;

        std::cout << "sent " << ( result ? "OK\n" : "FAIL\n");

        // close( fd );

        //return result;
        return fd;
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
        ASSERT_TRUE( tested_class->run() == loop<tsl_custom>::run_result::OK );
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
        ASSERT_TRUE( tested_class->run() == loop<std::nullptr_t>::run_result::OK );
    }

    ASSERT_TRUE( success );
}


TEST_F( test_server_loop_run, fn_on_client )
{
    ASSERT_TRUE( tested_class.operator bool() );

    int success_sent_cnt = 0;

    tested_class->input_strings =
        {
            std::string("test1"),
            std::string("test2"),
            std::string("test3")
        }
    ;

    std::thread thr
    (
        //std::launch::async,
        [&] ()  {
                    sleep( 1 );
                    std::list<int> fds;
                    for ( auto &s : tested_class->input_strings )
                    {
                        int tmp = send_string( s );
                        if ( tmp >= 0 )
                        {
                            ++success_sent_cnt;
                            fds.push_back( tmp );
                        }
                    }
                    sleep( 1 );
                    for ( auto &fd : fds )
                        ::close( fd );
                    std::cout << "try stop tested loop\n";
                    //ASSERT_TRUE( tested_class->stop() );
                    kill( getpid(), SIGINT );
                    sleep( 1 );
                    std::cout << "stop test thread\n";
                }
    );
    thr.detach();

    if ( tested_class )
    {
        std::cout << "RUN\n";
        ASSERT_TRUE( tested_class->run() == loop<std::nullptr_t>::run_result::OK );
        std::cout << "STOP\n";
    }

    ASSERT_EQ( tested_class->input_strings.size(), success_sent_cnt );
    ASSERT_EQ(
                tested_class->input_strings.size(),
                tested_class->output_strings.size()
    );
/*
    struct str_comparator
    {
        bool operator()(const std::string &a, const std::string &b)
        { return a < b; }
    } str_cmp;
*/
    std::sort(
                tested_class->output_strings.begin(),
                tested_class->output_strings.end()/*,
                str_cmp*/
             )
    ;

    unsigned int cnt = tested_class->input_strings.size();
    auto it_input = tested_class->input_strings.begin();
    auto it_output = tested_class->input_strings.begin();
    for ( unsigned int i = 0; i < cnt; i++ )
    {
        ASSERT_TRUE( *it_input == *it_output );
        it_input++;
        it_output++;
    }


}
