#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

namespace eventloop
{


class event_queue
{
public:
    // first is file descriptor, second is event
    struct item
    {
        int     fd;
        int     what;
        void    *data;
    };

private:
    std::deque<item>                queue;

    std::mutex                      mutex;

    std::mutex                      mutex_for_cv;
    std::unique_lock<std::mutex>    lock_for_cv;
    std::condition_variable         cv;

public:
    event_queue();
    ~event_queue() = default;

    void push(int fd, int what, void *custom_arg);
    // wait if empty
    item pop();
};


}

#endif // EVENT_QUEUE_H
