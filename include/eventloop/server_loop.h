/*
 * Copyright (c) 2021 Alexander Napylov.
 * BSD 2-clause license.
 */

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



/**
 *  @brief  Class to create server event loop.
 *  @tparam custom_data_t   Type for custom data for event.
 *  @tparam q_item          Type of events queue items.
 */
template <typename custom_data_t, typename q_item>
class server_loop : public server_loop_base<custom_data_t>
{
protected:
    /// Queue of events.
    event_queue<q_item>     queue;


public:
    /**
     * @brief server_loop   Default constructor.
     */
    server_loop() : server_loop_base<custom_data_t>() {}

    /**
     * @brief server_loop       Constructor.
     * @param port_             Listening port.
     * @param threads_count_    Threads count in a pool.
     */
    server_loop
    (
            uint16_t port_,
            int threads_count = server_loop_base<custom_data_t>::DEFAULT_THREADS_COUNT,
            timeval tv_ = {0,0}
    )
    : server_loop_base<custom_data_t>( port_, threads_count, tv_ )
    {}

    /**
     * @brief ~server_loop      Destructor.
     */
    virtual ~server_loop()
    {
        stop_threads();
    }

public:
    /**
     * @brief stop      Stop loop.
     * @return          True if success.
     */
    virtual bool stop() override
    {
        stop_threads();
        //DEBUG_CODE( std::cout << "Try loopexit\n" );
        return event_base_loopexit( this->base.get(), nullptr ) != -1;
    }

    /**
     * @brief stop_threads  Stop all threads.
     */
    virtual void stop_threads() override
    {
        FUNC;

        this->work_flag = false;
        queue.set_work_flag( false );

        for ( auto &it : this->threads )
            it->join();

        //DEBUG_CODE( std::cout << "stop_threads(): stopped\n" );

        this->threads.clear();
    }

protected:
    /**
     * @brief on_client     The callback function called for event on
     *                      client's connection socket.
     *                      The function may be overrided in child class.
     * @param fd            File desctiptor.
     * @param what          Flag of event (see libevent manual).
     * @param data          Custom data.
     */
    virtual void on_client( evutil_socket_t fd, short what, const custom_data_t &data ) override
    {
        //std::cout << __PRETTY_FUNCTION__ << "\n";
        //DEBUG_CODE( std::cout << "DEBUG fd[" << fd << "] what [" << what << "]\n" );
        if ( what & EV_READ )
            queue.push( q_item( fd, what, data ) );
        if ( what & EV_CLOSED )
        {
            LOG_DEBUG( "disconnect %d", fd );
            this->close_client_fd( fd );
        }
    }

    /**
     * @brief process_event Handling event.
     *                      The function have to overrided in child class.
     * @param item          Event's info.
     */
    virtual void process_event( q_item &&item ) = 0;

    /**
     * @brief process_thread_fn     Thread function.
     *                              The function pop event info from queue then
     *                              call process_event().
     */
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
