#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <unistd.h>
#include <string.h>

#include "eventloop/fd_factory.h"
#include "example_server_loop.h"


example_server_loop::example_server_loop()
    : example_server_loop_base()
{
}


example_server_loop::example_server_loop( uint16_t port )
    : example_server_loop_base( port )
{
}


example_custom_data example_server_loop::make_custom_data_on_accept
(
        evutil_socket_t         fd,
        const struct sockaddr   *addr,
        int                     sock_len
)
{
    return example_custom_data( fd, addr );
}


void example_server_loop::process_event( event_queue_item<example_custom_data> &&item )
{
    uint8_t buf[ 0x10000 ] = { 0 };

    /*
    printf( "Got event from socket %d host %s\n",
            item.fd, item.data.get_address().c_str() )
    ;
    */

    ssize_t received = recv( item.fd, buf, sizeof( buf ), 0 );
    if ( received <= 0 )
    {
        int err = errno;
        //printf( "Can't read from socket %d. errno %d\n", item.fd, err );
        if ( err != EAGAIN )
            close_client_fd( item.fd );
        return;
    }

    print_buf( buf, received );
    send( item.fd, buf, received, 0 );

    if ( ( received == 5 && strncmp( reinterpret_cast<char*>(buf), "exit\n", 5 ) == 0 ) ||
         ( received == 6 && strncmp( reinterpret_cast<char*>(buf), "exit\r\n", 6 ) == 0 )
       )
    {
        static const char BUY[] = "Buy!\n";
        send( item.fd, BUY, sizeof(BUY) - 1, 0 );
        close_client_fd( item.fd );
    }
}


void example_server_loop::print_buf( uint8_t *buf, ssize_t size )
{
    printf( "buffer %ld bytes\n", size );

    static const int BYTES_IN_LINE = 16;
    static const int HEX_SIZE = 3; // 2 digits + 1 space
    char line[ 3 * BYTES_IN_LINE + 1 ] = { 0 };

    uint8_t *end = buf + size;
    while ( buf < end )
    {
        int counter = 0;
        char *tmp = line;
        while ( counter < BYTES_IN_LINE && buf < end )
        {
            sprintf( tmp, "%02x ", static_cast<unsigned int>(*buf) );
            ++counter;
            ++buf;
            tmp += HEX_SIZE;
        }
        *tmp = '\0';
        puts( line );
    }
}


bool example_server_loop::init_custom_events()
{
    int signal_fd = fd_factory::create_signal_fd<std::array<int, 1> >( { SIGTERM } );
    if ( signal_fd < 0 )
    {
        LOG_ERROR( "Signal fd hasn't been inited." );
        return false;
    }

    // Make pointer to event object. Used file descriptor closer by default (close(int)).
    signal_event =
            make_event(
                signal_fd,
                EV_READ | EV_PERSIST,
                std::bind(
                    &example_server_loop::on_signal,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3
                ),
                nullptr,
                example_custom_data()
            )
    ;

    return signal_event.operator bool();
}


void example_server_loop::on_signal(int fd, int, const example_custom_data &)
{
    signalfd_siginfo sig_info = { 0 };
    if ( read( fd, &sig_info, sizeof(sig_info) ) <
         static_cast<ssize_t>( sizeof( sig_info ) )
    )
    {
        std::cout << "Can't read signal.\n";
        return;
    }

    std::cout << "Got signal " << sig_info.ssi_signo << "\n";
    stop();
}
