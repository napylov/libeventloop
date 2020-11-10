#ifndef LOOP_H
#define LOOP_H

#include <functional>
#include "types.h"

namespace eventloop
{

class loop
{
private:
    event_cfg_ptr               cfg;
    event_base_ptr              base;

public:
    typedef std::function<void(loop &,int, short, void*)>  callback_fn;

    struct callback_info
    {
        loop            *obj    = nullptr;
        callback_fn     fn;
        void            *arg    = nullptr;

        callback_info() = delete;
        callback_info( loop *obj_, callback_fn fn_, void *arg_ )
            : obj( obj_ ), fn( fn_ ), arg( arg_ ) {}
        ~callback_info() = default;
    };

public:
    loop() = default;
    virtual ~loop() = default;

    inline static void call_callback( int fd, short what, callback_info *info )
    {
        if ( info && info->obj )
            info->fn(*(info->obj), fd, what, info->arg);
    }

private:
    static event_cfg_ptr    make_config();
    static event_base_ptr   make_base();
    static event_base_ptr   make_base( event_cfg_ptr &cfg );

    static event_ptr        make_event(
                                loop *obj,
                                event_base_ptr &base,
                                int fd,
                                short what,
                                void *arg,
                                callback_fn fn,
                                const timeval *tv = nullptr
                            )
    ;
};

}; // namespace eventloop;

#endif // LOOP_H
