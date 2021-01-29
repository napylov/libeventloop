#ifndef QUEUE_ITEM_H
#define QUEUE_ITEM_H


namespace eventloop
{

// Default type to use event_queue.
// Custom data are ignored.
template <typename custom_data_t>
struct event_queue_item
{
    int             fd;
    int             what;

    event_queue_item() : fd( -1 ), what( 0 ) {}
    event_queue_item( int fd_, int what_, const custom_data_t& /* need to interface. ignored*/ )
        : fd( fd_ ), what( what_ )
        {}

    virtual ~event_queue_item() = default;
};


}

#endif // QUEUE_ITEM_H
