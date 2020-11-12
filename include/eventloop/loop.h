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
        loop            *obj;
        callback_fn     fn;
        void            *arg;

        callback_info() = delete;
        callback_info( loop *obj_, callback_fn fn_, void *arg_ )
            : obj( obj_ ), fn( fn_ ), arg( arg_ ) {}
        ~callback_info() = default;
    };


    static const int RUN_ERROR  = -1;
    static const int OK         = 0;
    static const int NO_EVENTS  = 1;

public:
    loop() = default;
    virtual ~loop() = default;

    int run();
    bool stop();

public:
    inline static void call_callback( int fd, short what, callback_info *info )
    {
        if ( info && info->obj && info->fn )
            info->fn(*(info->obj), fd, what, info->arg);
    }



private:
    bool        make_config();
    bool        make_base();

    event_ptr   make_event
                (
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
