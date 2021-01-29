#ifndef SERVER_LOOP_H
#define SERVER_LOOP_H

#include <iostream>

#include "server_loop_base.h"
#include "stdint.h"
#include "event_queue.h"
#include "event_queue_item.h"

#include "debug_print.h"

namespace eventloop
{


template <typename custom_data_t, typename q_item>
class server_loop : public server_loop_base<custom_data_t>
{
protected:
    event_queue<q_item>     queue;


public:
    server_loop() : server_loop_base<custom_data_t>() {}
    server_loop
    (
            uint16_t port_,
            int threads_count = server_loop_base<custom_data_t>::DEFAULT_THREADS_COUNT,
            timeval tv_ = {0,0}
    )
    : server_loop_base<custom_data_t>( port_, threads_count, tv_ )
    {}

    virtual ~server_loop()
    {
        stop_threads();
    }

public:
    virtual bool stop() override
    {
        stop_threads();
        //DEBUG_CODE( std::cout << "Try loopexit\n" );
        return event_base_loopexit( this->base.get(), nullptr ) != -1;
    }

    virtual void stop_threads() override
    {
        this->work_flag = false;
        queue.set_work_flag( false );

        for ( auto &it : this->threads )
            it->join();

        //DEBUG_CODE( std::cout << "stop_threads(): stopped\n" );

        this->threads.clear();
    }

protected:
    virtual void on_client( evutil_socket_t fd, short what, const custom_data_t &data ) override
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
        //DEBUG_CODE( std::cout << "DEBUG fd[" << fd << "] what [" << what << "]\n" );
        if ( what & EV_READ )
            queue.push( q_item( fd, what, data ) );
        else if ( what & EV_CLOSED )
            this->fd_events.erase( fd );
    }

    virtual void process_event( q_item &&item ) = 0;

    virtual void process_thread_fn()
    {
        while ( this->work_flag )
        {
            process_event( queue.pop() );
        }
        //DEBUG_CODE( std::cout << __PRETTY_FUNCTION__ << " FINISHED\n" );
    }
};


}

#endif // SERVER_LOOP_H
