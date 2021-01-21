#ifndef FD_FACTORY_H
#define FD_FACTORY_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include <string>
#include <memory>
#include <functional>

#include <iostream>

class fd_factory
{
public:
    static const int INVALID_FD = -1;

    typedef std::unique_ptr< addrinfo, std::function<void(addrinfo*)> >
        addrinfo_ptr
    ;

public:
    static addrinfo_ptr get_addrinfo( const char *host, int proto );

    static int connect( const char *host, uint16_t port, int proto );

    static inline int connect( const std::string &host, uint16_t port, int proto )
    {
        return connect( host.c_str(), port, proto );
    }


    template <class T>
    static bool init_sigset( sigset_t *set, const T &container )
    {
        if ( !set )
            return false;

        sigemptyset( set );

        bool result = true;
        for ( auto &it : container )
        {
            if ( sigaddset( set, it ) < 0 )
            {
                result = false;
                break;
            }
        }

        if ( !result )
            sigemptyset( set );

        sigprocmask( SIG_SETMASK, set, nullptr );

        return result;
    }


    template <class T>
    static int create_signal_fd( const T &container )
    {
        sigset_t sigset = { 0 };
        if ( !init_sigset( &sigset, container ) )
            return INVALID_FD;

        return signalfd( -1, &sigset, SFD_NONBLOCK );
    }
};

#endif // FD_FACTORY_H
