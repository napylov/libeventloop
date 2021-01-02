#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

namespace eventloop
{


template <typename item>
class event_queue
{
public:

private:
    std::deque<item>                queue;

    std::mutex                      mutex;

    std::mutex                      mutex_for_cv;
    std::unique_lock<std::mutex>    lock_for_cv;
    std::condition_variable         cv;

public:
    event_queue()
    :   queue(),
        mutex(),
        mutex_for_cv(),
        lock_for_cv( mutex_for_cv, std::defer_lock ),
        cv()
    {
    }

    ~event_queue() = default;

    void push( item &&obj )
    {
        std::unique_lock<std::mutex> lock( mutex );

        queue.emplace_back( obj );

        lock.unlock();

        cv.notify_one();
    }


    void push( const item &obj )
    {
        std::unique_lock<std::mutex> lock( mutex );

        queue.push_back( obj );

        lock.unlock();

        cv.notify_one();
    }

    // wait if empty
    item pop()
    {
        bool ok = false;
        while ( !ok )
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
                cv.wait( lock_for_cv );
        }

        return item();
    }
};


}

#endif // EVENT_QUEUE_H
