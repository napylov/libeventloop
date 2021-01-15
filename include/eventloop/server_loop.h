#ifndef SERVER_LOOP_H
#define SERVER_LOOP_H

#include <iostream>

#include "server_loop_base.h"
#include "stdint.h"
#include "event_queue.h"
#include "event_queue_item.h"


namespace eventloop
{


template <typename q_item = event_queue_item>
class server_loop : public server_loop_base
{
protected:
    event_queue<q_item>     queue;


public:
    server_loop() : server_loop_base() {}
    server_loop
    (
            uint16_t port_,
            int threads_count = server_loop_base::DEFAULT_THREADS_COUNT,
            timeval tv_ = {0,0}
    )
    : server_loop_base( port_, threads_count, tv_ )
    {}

    virtual ~server_loop()
    {
        stop_threads();
    }

public:
    virtual bool stop() override
    {
        stop_threads();
        return event_base_loopexit( base.get(), nullptr ) != -1;
    }

    virtual void stop_threads() override
    {
        work_flag = false;
        queue.set_work_flag( false );

        for ( auto &it : threads )
            it->join();

        threads.clear();
    }

protected:
    virtual void on_client( evutil_socket_t fd, short what )
    {
        if ( what & EV_READ )
            queue.push( q_item( fd, what, nullptr ) );
        else if ( what & EV_CLOSED )
            fd_events.erase( fd );
    }

    virtual void process_event( q_item &&item ) = 0;

    virtual void process_thread_fn()
    {
        while ( work_flag )
        {
            process_event( queue.pop() );
        }
    }
};


}

#endif // SERVER_LOOP_H
