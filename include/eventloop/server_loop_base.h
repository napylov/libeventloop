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
    server_loop_base();
    server_loop_base
    (
            uint16_t port_,
            int threads_count_ = DEFAULT_THREADS_COUNT,
            timeval tv_ = {0,0}
    );
    virtual ~server_loop_base();

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
    virtual bool make_listener();
    virtual callback_accept_info* make_callback_accept_info();

public:
    void run_threads( int cnt );
    virtual void stop_threads();

public:
    virtual bool init() override;

protected:
    virtual void on_accept
    (
            evutil_socket_t         fd,
            const struct sockaddr   *addr,
            int                     sock_len
    );

protected:
    virtual void on_client( evutil_socket_t fd, short what ) = 0;
    virtual void process_thread_fn() = 0;
};


} // namespace eventloop


#endif // SERVER_LOOP_BASE_H
