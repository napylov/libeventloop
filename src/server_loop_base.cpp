#include <string.h>
#include "eventloop/server_loop_base.h"

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
    server_loop_base::call_callback
        (
            listener,
            socket,
            addr,
            len,
            reinterpret_cast<server_loop_base::callback_accept_info*>(ptr)
        )
    ;
}



server_loop_base::server_loop_base()
    : loop(), port( DEFAULT_PORT ), threads_count( DEFAULT_THREADS_COUNT ), work_flag( false )
{
}


server_loop_base::server_loop_base( uint16_t port_, int threads_count_, timeval tv_ )
    : loop(), port( port_ ), threads_count( threads_count_ ), work_flag( false )
{
    if ( tv_.tv_sec != 0 && tv_.tv_usec != 0 )
    {
        timeval *new_tv = new timeval;
        memcpy( new_tv, &tv_, sizeof( tv_ ) );
    }
}


server_loop_base::~server_loop_base()
{
    stop_threads();
}


bool server_loop_base::init()
{
    if ( !(make_config() && make_base() && make_listener()) )
        return false;
    run_threads( threads_count );

    return true;
}


bool server_loop_base::make_listener()
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


void server_loop_base::run_threads( int cnt )
{
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


void server_loop_base::stop_threads()
{
    work_flag = false;

    for ( auto &it : threads )
        it->join();

    threads.clear();
}


server_loop_base::callback_accept_info* server_loop_base::make_callback_accept_info()
{
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
    catch ( std::bad_alloc & )
    {
        delete info;
        info = nullptr;
    }

    return info;
}


void server_loop_base::on_accept
(
        evutil_socket_t         fd,
        const struct sockaddr   *addr,
        int                     sock_len
)
{
    fd_events[ fd ] =
        make_event
            (
                fd,
                EV_READ | EV_CLOSED,
                std::bind
                (
                    &server_loop_base::on_client,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2
                ),
                tv.get()
            )
    ;
}


} // namespace eventloop
