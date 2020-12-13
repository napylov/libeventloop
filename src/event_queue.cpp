#include "eventloop/event_queue.h"

namespace eventloop
{


event_queue::event_queue()
:   queue(),
    mutex(),
    mutex_for_cv(),
    lock_for_cv( mutex_for_cv, std::defer_lock ),
    cv()
{
}


void event_queue::push( int fd, int what, void *custom_arg )
{
    std::unique_lock<std::mutex> lock( mutex );

    queue.emplace_back( item{ fd, what, custom_arg } );

    lock.unlock();

    cv.notify_one();
}


event_queue::item event_queue::pop()
{
    item result;
    bool ok = false;

    while ( !ok )
    {
        std::unique_lock<std::mutex> lock( mutex );

        ok = !queue.empty();
        if ( ok )
        {
            result = queue.front();
            queue.pop_front();
        }

        lock.unlock();

        if ( !ok )
            cv.wait( lock_for_cv );
    }

    return result;
}


}
