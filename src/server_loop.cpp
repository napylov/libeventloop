#include <string.h>
#include "eventloop/server_loop.h"

namespace eventloop
{


void evconnlistener_deleter( evconnlistener *obj )
{
    if ( obj )
        evconnlistener_free( obj );
}


void evconnlistener_callback
        (
            struct evconnlistener *listener,
            evutil_socket_t socket,
            struct sockaddr *addr,
            int len,
            void *ptr
        )
{
    server_loop::call_callback
        (
            listener,
            socket,
            addr,
            len,
            reinterpret_cast<server_loop::callback_accept_info*>(ptr)
        )
    ;
}



server_loop::server_loop()
    : loop(), port( DEFAULT_PORT )
{
    run_threads( DEFAULT_THREADS_COUNT );
}


server_loop::server_loop( uint16_t port_, int threads_count, timeval tv_ )
    : loop(), port( port_ )
{
    if ( tv_.tv_sec != 0 && tv_.tv_usec != 0 )
    {
        timeval *new_tv = new timeval;
        memcpy( new_tv, &tv_, sizeof( tv_ ) );
    }
    run_threads( threads_count );
}


bool server_loop::init()
{
    return make_config() && make_base() && make_listener();
}


bool server_loop::make_listener()
{
    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;    /* работа с доменом IP-адресов */
    sin.sin_addr.s_addr = INADDR_ANY;  /* принимать запросы с любых адресов */
    sin.sin_port = htons( port );

    listener = evconnlistener_ptr
                (
                    evconnlistener_new_bind
                    (
                        base.get(),
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

    return listener.operator bool();
}


void server_loop::run_threads( int cnt )
{
    for ( int i = 0; i < cnt; i++ )
    {
        thread_info info =
            std::make_pair
            (
                thread_ptr(),
                std::unique_ptr<std::atomic_bool>( new std::atomic_bool( true ) )
            )
        ;
        info.first.reset
            (
                new std::thread
                    ( [&] () { this->process_thread_fn( *(info.second) ); } )
            )
        ;

        threads.emplace_back( std::move( info ) );
    }
}


server_loop::callback_accept_info* server_loop::make_callback_accept_info()
{
    callback_accept_info* info = nullptr;
    try
    {
        info = new callback_accept_info
                        (
                            this,
                            std::bind
                            (
                                &server_loop::on_accept,
                                this,
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3,
                                std::placeholders::_4
                            ),
                            nullptr
                        )
        ;
    }
    catch ( std::bad_alloc & )
    {
    }

    return info;
}


void server_loop::on_accept
(
        evutil_socket_t         fd,
        struct sockaddr         *addr,
        int                     sock_len,
        void                    *arg
)
{
    fd_events[ fd ] =
        make_event
            (
                fd,
                EV_READ | EV_CLOSED,
                NULL,
                std::bind
                (
                    &server_loop::on_client,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3
                ),
                tv.get()
            )
    ;
}


void server_loop::process_thread_fn( std::atomic_bool &work_flag )
{
    while ( !work_flag.load() )
    {
        event_queue::item item = queue.pop();
        process_data( item );
    }
}


} // namespace eventloop
