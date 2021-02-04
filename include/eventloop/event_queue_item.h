#ifndef QUEUE_ITEM_H
#define QUEUE_ITEM_H


namespace eventloop
{

// Default type to use event_queue.
template <typename custom_data_t>
struct event_queue_item
{
    int             fd;
    int             what;
    custom_data_t   data;

    event_queue_item() : fd( -1 ), what( 0 ) {}
    event_queue_item( int fd_, int what_, const custom_data_t& data_ )
        : fd( fd_ ), what( what_ ), data( data_ )
        {}

    virtual ~event_queue_item() = default;
};


}

#endif // QUEUE_ITEM_H
