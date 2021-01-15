#include <event2/event.h>
#include "eventloop/loop.h"

namespace eventloop
{


static void config_destructor( event_config *cfg )
{
    if ( cfg )
        event_config_free( cfg );
}


static void base_destructor( event_base *base )
{
    if ( base )
        event_base_free( base );
}


static void event_destructor( event *ev )
{
    if ( !ev )
        return;

    void *arg = event_get_callback_arg( ev );
    if ( arg )
    {
        loop::callback_info *info = reinterpret_cast<loop::callback_info*>( arg );
        delete info;
    }

    event_del( ev );
    event_free( ev );
}


static void event_callback( int fd, short flags, void *arg )
{
    loop::call_callback
                (
                    fd,
                    flags,
                    reinterpret_cast<loop::callback_info*>( arg )
                );
}


int loop::run()
{
    return event_base_dispatch( base.get() );
}


bool loop::stop()
{
    return event_base_loopexit( base.get(), nullptr ) != -1;
}


bool loop::make_config()
{
    cfg = event_cfg_ptr( event_config_new(), &config_destructor );
    return cfg.operator bool();
}


bool loop::make_base()
{
    if ( cfg )
    {
        base = event_base_ptr
                    (
                        event_base_new_with_config( cfg.get() ),
                        &base_destructor
                    )
        ;
    }
    else
        base = event_base_ptr( event_base_new(), &base_destructor );

    return base.operator bool();
}


event_ptr loop::make_event(
                                int fd,
                                short what,
                                callback_fn fn,
                                const timeval *tv
                           )
{
    loop::callback_info *info = make_callback_info( fn );
    if ( !info )
        return event_ptr();

    event_ptr result
                (
                    event_new( base.get(), fd, what, &event_callback, info ),
                    &event_destructor
                )
    ;

    if ( result )
        event_add( result.get(), tv );

    return result;
}


loop::callback_info *loop::make_callback_info( callback_fn fn )
{
    loop::callback_info *info = nullptr;
    try
    {
        info = new loop::callback_info( fn );
    }
    catch ( std::bad_alloc & )
    {
    }

    return info;
}


} // namespace eventloop
