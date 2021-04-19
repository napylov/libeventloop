/*
 * Copyright (c) 2021 Alexander Napylov.
 * BSD 2-clause license.
 */

#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <atomic>
#include <iostream>
#include <deque>
#include <mutex>
#include <condition_variable>

namespace eventloop
{

/**
 * @brief   event_queue     Thread safety queue of events.
 * @tparam  item            Type of queue item.
 */
template <typename item>
class event_queue
{
public:

private:
    std::deque<item>                queue;

    std::mutex                      mutex;

    std::mutex                      mutex_for_cv;
    std::condition_variable         cv;

    std::atomic_bool                work_flag;

public:
    event_queue()
    :   queue(),
        mutex(),
        mutex_for_cv(),
        cv(),
        work_flag( true )
    {
    }

    ~event_queue() = default;

    /**
     * @brief push  Push event into queue.
     * @param obj   Object to push.
     */
    void push( item &&obj )
    {
        std::unique_lock<std::mutex> lock( mutex );

        queue.emplace_back( obj );

        lock.unlock();

        notify();
    }


    /**
     * @brief push  Push event into queue.
     * @param obj   Object to push.
     */
    void push( const item &obj )
    {
        std::unique_lock<std::mutex> lock( mutex );

        queue.push_back( obj );

        lock.unlock();

        notify();
    }

    /**
     * @brief pop   Pop event from queue.
     *              If queue is empty and work_flag is not reset
     *              the function wait.
     * @return      Item from queue.
     */
    item pop()
    {
        bool ok = false;
        while ( !ok && work_flag.load() )
        {
            std::unique_lock<std::mutex> lock( mutex );

            ok = !queue.empty();
            if ( ok )
            {
                item result = queue.front();
                queue.pop_front();
                return result;
            }

            lock.unlock();

            if ( !ok )
            {
                std::unique_lock<std::mutex> lk( mutex_for_cv );
                cv.wait_for( lk, std::chrono::seconds(1) );
            }
        }

        return item();
    }

    /*
    bool pop( item &obj, bool wait )
    {
    }
    */


    /**
     * @brief set_work_flag
     * @param value         New value of a flag.
     */
    void set_work_flag( bool value )
    {
        work_flag.store( value );
        if ( !value )
        {
            std::lock_guard<std::mutex> lk( mutex_for_cv );
            cv.notify_all();
        }
    }


private:
    inline void notify()
    {
        std::lock_guard<std::mutex> lk( mutex_for_cv );
        cv.notify_one();
    }
};


}

#endif // EVENT_QUEUE_H
