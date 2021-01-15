#ifndef QUEUE_ITEM_H
#define QUEUE_ITEM_H


namespace eventloop
{


struct event_queue_item
{
    int     fd;
    int     what;
    void    *data;

    event_queue_item() : fd( -1 ), what( 0 ), data( nullptr ) {}
    event_queue_item( int fd_, int what_, void *data_ )
        : fd( fd_ ), what( what_ ), data( data_ )
        {}

    virtual ~event_queue_item() = default;
};


}

#endif // QUEUE_ITEM_H
