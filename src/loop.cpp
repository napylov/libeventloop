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


event_cfg_ptr loop::make_config()
{
    return event_cfg_ptr( event_config_new(), &config_destructor );
}


event_base_ptr loop::make_base()
{
    return event_base_ptr( event_base_new(), &base_destructor );
}


event_base_ptr loop::make_base( event_cfg_ptr &cfg )
{
    return event_base_ptr
                (
                    event_base_new_with_config( cfg.get() ),
                    &base_destructor
                )
    ;
}


event_ptr loop::make_event(
                                loop *obj,
                                event_base_ptr &base,
                                int fd,
                                short what,
                                void *arg,
                                callback_fn fn,
                                const timeval *tv
                           )
{
    loop::callback_info *info = nullptr;
    try
    {
        info = new loop::callback_info( obj, fn, arg );
    }
    catch ( std::bad_alloc & )
    {
        return event_ptr();
    }

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


}
