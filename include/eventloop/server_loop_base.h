#ifndef SERVER_LOOP_BASE_H
#define SERVER_LOOP_BASE_H

#include <map>
#include <thread>
#include <list>
#include <atomic>

#include "loop.h"

namespace eventloop
{


class server_loop_base : public loop
{
public:
    static const short  DEFAULT_PORT            = 3000;
    static const int    DEFAULT_THREADS_COUNT   = 4;

public:
    typedef std::function<
                void(
                        evutil_socket_t,
                        struct sockaddr *,
                        int,
                        void *
                )
            >  callback_accept_fn
    ;

    struct callback_accept_info
    {
        server_loop_base    *obj;
        callback_accept_fn  fn;
        void                *arg;

        callback_accept_info() = delete;
        callback_accept_info( server_loop_base *obj_, callback_accept_fn fn_, void *arg_ )
            : obj( obj_ ), fn( fn_ ), arg( arg_ ) {}
        virtual ~callback_accept_info() = default;
    };

    typedef std::unique_ptr<timeval>                    timeval_ptr;

    typedef std::unique_ptr<std::thread>                thread_ptr;
    typedef std::pair<thread_ptr, std::unique_ptr<std::atomic_bool> >
            thread_info
    ;

protected:
    evconnlistener_ptr          listener;
    std::map<int, event_ptr>    fd_events;

    uint16_t                    port;
    std::unique_ptr<timeval>    tv;

    std::list<thread_info>       threads;

public:
    server_loop_base();
    server_loop_base
    (
            uint16_t port_,
            int threads_count = DEFAULT_THREADS_COUNT,
            timeval tv_ = {0,0}
    );
    virtual ~server_loop_base() = default;

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
            info->fn( fd, addr, sock_len, info->arg );
        }
    }


protected:
    virtual bool make_listener();
    virtual callback_accept_info* make_callback_accept_info();
    void run_threads( int cnt );

private:
    virtual bool init();

    virtual void on_accept
    (
            evutil_socket_t         fd,
            struct sockaddr         *addr,
            int                     sock_len,
            void                    *arg
    );

protected:
    virtual void on_client( evutil_socket_t fd, short what, void *arg ) = 0;
    virtual void process_thread_fn( std::atomic_bool &work_flag ) = 0;
};


} // namespace eventloop


#endif // SERVER_LOOP_BASE_H
