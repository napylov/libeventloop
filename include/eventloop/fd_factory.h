/*
 * Copyright (c) 2021 Alexander Napylov.
 * BSD 2-clause license.
 */

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


namespace eventloop
{


/**
 * @brief   The fd_factory class
 *          Factory to create some kinds of file descriptors.
 */
class fd_factory
{
public:
    static const int INVALID_FD = -1;

    typedef std::unique_ptr< addrinfo, std::function<void(addrinfo*)> >
        addrinfo_ptr
    ;

public:
    /**
     * @brief get_addrinfo  Create addrinfo pointer from host and proto.
     * @param host          Host.
     * @param proto         Protocol (see socket library).
     * @return              Pointer (unique_ptr) to addrinfo object.
     */
    static addrinfo_ptr get_addrinfo( const char *host, int proto );


    /**
     * @brief connect       Connect to server.
     * @param host          Host.
     * @param port          Port.
     * @param proto         Protocol.
     * @return
     */
    static int connect( const char *host, uint16_t port, int proto );


    /**
     * @brief connect       Connect to server.
     * @param host          Host.
     * @param port          Port.
     * @param proto         Protocol.
     * @return
     */
    static inline int connect( const std::string &host, uint16_t port, int proto )
    {
        return connect( host.c_str(), port, proto );
    }


    /**
     * @brief set_fd_non_block  Set non-block flags for socket.
     * @param fd                File descriptor.
     * @return                  true if success.
     */
    static bool set_fd_non_block( int fd );


    /**
     * @brief  init_sigset      Init sigset_t object to create file descriptor
     *                          for signals handling.
     * @tparam T                Type of container with signals numbers.
     * @param  set              sigset_t object.
     * @param container         Container with signals numbers.
     * @return                  true if success.
     */
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


    /**
     * @brief   create_signal_fd    Create file descriptor for signals handling.
     * @tparam  T                   Type of container with signals numbers.
     * @param                       Container with signals numbers.
     */
    template <class T>
    static int create_signal_fd( const T &container )
    {
        sigset_t sigset = { 0 };
        if ( !init_sigset( &sigset, container ) )
            return INVALID_FD;

        return signalfd( -1, &sigset, SFD_NONBLOCK );
    }
};


} // namespace eventloop

#endif // FD_FACTORY_H
