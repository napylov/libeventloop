/*
 * Copyright (c) 2021 Alexander Napylov.
 * BSD 2-clause license.
 */

#ifndef SERVER_LOOP_BASE_H
#define SERVER_LOOP_BASE_H

#include <map>
#include <thread>
#include <list>
#include <atomic>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "loop.h"
#include "debug_print.h"
#include "tools.h"

namespace eventloop
{


/**
 *  Base class to make event loop for server.
 */
template <typename custom_data_t>
class server_loop_base : public loop<custom_data_t>
{
public:
    static const uint16_t   DEFAULT_PORT            = 3000;
    static const int        DEFAULT_THREADS_COUNT   = 4;

public:
    typedef std::function<
                void(
                        evutil_socket_t,
                        struct sockaddr *,
                        int
                )
            >  callback_accept_fn
    ;

    struct callback_accept_info
    {
        server_loop_base    *obj;
        callback_accept_fn  fn;

        callback_accept_info() = delete;
        callback_accept_info( server_loop_base *obj_, callback_accept_fn fn_ )
            : obj( obj_ ), fn( fn_ ) {}
        virtual ~callback_accept_info() = default;
    };

    typedef std::unique_ptr<timeval>                    timeval_ptr;

    typedef std::unique_ptr<std::thread>                thread_ptr;

protected:
    evconnlistener_ptr          listener;
    std::map<int, event_ptr>    fd_events;

    uint16_t                    port;
    int                         threads_count;
    std::unique_ptr<timeval>    tv;

    std::list<thread_ptr>       threads;
    bool                        work_flag;

public:
    /**
     * @brief server_loop_base  Constructor.
     */
    server_loop_base()
    :   loop<custom_data_t>(),
        port( DEFAULT_PORT ),
        threads_count( DEFAULT_THREADS_COUNT ),
        work_flag( false )
    {
    }


    /**
     * @brief server_loop_base  Constructor.
     * @param port_             Listen port.
     * @param thread_count      Size of threads pool.
     */
    server_loop_base
    (
            uint16_t port_,
            int threads_count_ = DEFAULT_THREADS_COUNT,
            timeval tv_ = {0,0}
    )
    :   loop<custom_data_t>(),
        port( port_ ),
        threads_count( threads_count_ ),
        work_flag( false )
    {
        if ( tv_.tv_sec != 0 && tv_.tv_usec != 0 )
        {
            timeval *new_tv = new timeval;
            memcpy( new_tv, &tv_, sizeof( tv_ ) );
        }
    }


    virtual ~server_loop_base()
    {
        stop_threads();
    }


public:
    inline static void call_callback
        (
            evconnlistener          *connlistener,
            evutil_socket_t         fd,
            struct sockaddr         *addr,
            int                     sock_len,
            callback_accept_info    *info
        )
    {
        if (
                info &&
                info->obj &&
                info->fn &&
                connlistener == info->obj->listener.get()
            )
        {
            info->fn( fd, addr, sock_len );
        }
    }


protected:
    virtual bool make_listener()
    {
        FUNC;

        struct sockaddr_in6 sin = {0};
        sin.sin6_family = AF_INET6;
        sin.sin6_addr = IN6ADDR_ANY_INIT;
        sin.sin6_port = htons( port );

        listener = evconnlistener_ptr
                    (
                        evconnlistener_new_bind
                        (
                            loop<custom_data_t>::base.get(),
                            &evconnlistener_callback,
                            make_callback_accept_info(),
                            LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC | LEV_OPT_REUSEABLE,
                            -1,
                            reinterpret_cast<struct sockaddr *>(&sin),
                            sizeof(sin)
                        ),
                        &evconnlistener_deleter
                    )
        ;

        LOG_DEBUG( "lister %d", (int)listener.operator bool() );

        return listener.operator bool();
    }

    virtual callback_accept_info* make_callback_accept_info()
    {
        FUNC;

        callback_accept_info* info = nullptr;
        try
        {
            info = new callback_accept_info
                            (
                                this,
                                std::bind
                                (
                                    &server_loop_base::on_accept,
                                    this,
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3
                                )
                            )
            ;
        }
        catch ( std::bad_alloc &e )
        {
            LOG_ERROR( e.what() );
            delete info;
            info = nullptr;
        }

        return info;
    }


public:
    void run_threads( int cnt )
    {
        FUNC;

        work_flag = true;
        for ( int i = 0; i < cnt; i++ )
        {
            threads.emplace_back
                    (
                        std::unique_ptr<std::thread>(
                            new std::thread( [&] () { this->process_thread_fn(); } )
                        )
                    )
            ;
        }
    }


    virtual void stop_threads()
    {
        FUNC;

        work_flag = false;

        for ( auto &it : threads )
            it->join();

        threads.clear();
    }


public:
    virtual bool init() override
    {
        FUNC;

        if ( !(
                 this->make_config() &&
                 this->make_base() &&
                 this->make_listener() &&
                 this->init_custom_events()
               )
            )
        {
            return false;
        }
        run_threads( threads_count );

        return true;
    }

    virtual bool init_custom_events()
    {
        return true;
    }


public:
    /*
    static std::string get_ip_str( const struct sockaddr *sa )
    {
        static const int BUF_SIZE = 255;
        char s[ BUF_SIZE + 1 ] = { 0 };
        switch(sa->sa_family) {
            case AF_INET:
                inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
                        s, BUF_SIZE);
                break;

            case AF_INET6:
                inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
                        s, BUF_SIZE);
                break;

            default:
                break;
        }

        return std::string(s);
    }
    */


protected:
    /**
     * @brief on_accept     Callback function called when new client connecting to server.
     * @param fd            File descriptor.
     * @param addr          Address.
     * @param sock_len      addr size.
     */
    virtual void on_accept
    (
            evutil_socket_t         fd,
            const struct sockaddr   *addr,
            int                     sock_len
    )
    {
        FUNC;

        DEBUG_CODE( \
            std::cout << "fd [" << fd << "]\n"; \
            std::cout << "address [" << tools::get_ip_str( addr ) << "]\n"; \
        );

        fd_events[ fd ] =
            this->make_event
                (
                    fd,
                    EV_READ | EV_CLOSED | EV_PERSIST,
                    std::bind
                    (
                        &server_loop_base::call_on_client,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2,
                        std::placeholders::_3
                    ),
                    tv.get(),
                    make_custom_data_on_accept( fd, addr, sock_len )
                )
        ;
    }


    virtual custom_data_t make_custom_data_on_accept
    (
            evutil_socket_t         fd,
            const struct sockaddr   *addr,
            int                     sock_len
    )
    {
        return custom_data_t();
    }


protected:
    inline void call_on_client( evutil_socket_t fd, short what, const custom_data_t &data )
    {
        on_client( fd, what, data );
    }


    /**
     * @brief on_client     Callback function called when happen
     *                      event in client connection socket.
     *                      The function have to implement in derived class.
     * @param fd            File descriptors.
     * @param what          Flags (see libevent's manual).
     */
    virtual void on_client( evutil_socket_t fd, short what, const custom_data_t & ) = 0;

    /**
     * @brief process_thread_fn     Thread's function.
     *                              The function have implement in derived class.
     */
    virtual void process_thread_fn() = 0;


    virtual void close_client_fd( int fd )
    {
        FUNC;
        fd_events.erase( fd );
    }


public:
    static void evconnlistener_deleter( evconnlistener *obj )
    {
        if ( obj )
            evconnlistener_free( obj );
    }


    static void evconnlistener_callback
            (
                struct evconnlistener *listener,
                evutil_socket_t socket,
                struct sockaddr *addr,
                int len,
                void *ptr
            )
    {
        server_loop_base::call_callback
            (
                listener,
                socket,
                addr,
                len,
                reinterpret_cast<callback_accept_info*>(ptr)
            )
        ;
    }

};


} // namespace eventloop


#endif // SERVER_LOOP_BASE_H
