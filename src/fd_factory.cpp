#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

#include "eventloop/fd_factory.h"
#include "eventloop/debug_print.h"


fd_factory::addrinfo_ptr fd_factory::get_addrinfo( const char *host, int proto )
{
    addrinfo request = { 0 };
    request.ai_family = proto;

    addrinfo *info = nullptr;

    if ( ::getaddrinfo( host, nullptr, &request, &info ) != 0 )
        return fd_factory::addrinfo_ptr();

    return fd_factory::addrinfo_ptr
            (
                info,
                [] ( addrinfo *info ) { ::freeaddrinfo( info ); }
            )
    ;
}


int fd_factory::connect( const char *host, uint16_t port, int proto )
{
    int fd = socket( proto, SOCK_STREAM, 0 );
    if ( fd == INVALID_FD )
        return INVALID_FD;

    DEBUG_CODE( std::cout << "fd OK\n"; );

    if ( !set_fd_non_block( fd ) )
    {
        DEBUG_CODE( std::cout << "can't set O_NONBLOCK\n"; );
        return false;
    }

    addrinfo_ptr info = get_addrinfo( host, proto );
    addrinfo *cur_info = info.get();
    bool success = false;

    while ( cur_info && !success )
    {
        if ( cur_info->ai_family == AF_INET )
        {
            DEBUG_CODE( std::cout << "AF_INET\n"; );
            (reinterpret_cast<sockaddr_in*>( cur_info->ai_addr ))->sin_port =
                htons( port )
            ;
        }
        else
        {
            DEBUG_CODE( std::cout << "AF_INET6\n"; );
            (reinterpret_cast<sockaddr_in6*>( cur_info->ai_addr ))->sin6_port =
                htons( port )
            ;
        }
        success = ::connect( fd, cur_info->ai_addr, cur_info->ai_addrlen ) == 0;
        DEBUG_CODE( std::cout << "success: " << success << "\n"; );

        cur_info = cur_info->ai_next;
    }

    if ( !success )
    {
        ::close( fd );
        return INVALID_FD;
    }

    return fd;
}


bool fd_factory::set_fd_non_block( int fd )
{
    int flags = fcntl( fd, F_GETFL );
    if ( flags < 0 )
        return false;

    return fcntl( fd, F_SETFL, flags | O_NONBLOCK ) != -1;
}
