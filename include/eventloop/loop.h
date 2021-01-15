#ifndef LOOP_H
#define LOOP_H

#include <functional>
#include <utility>

#include "types.h"

namespace eventloop
{


class loop
{
protected:
    event_cfg_ptr               cfg;
    event_base_ptr              base;

public:
    typedef std::function<void(int, short)>  callback_fn;

    struct callback_info
    {
        callback_fn     fn;

        callback_info() = delete;
        callback_info( callback_fn fn_ )
            : fn( fn_ ) {}
        virtual ~callback_info() = default;
    };


    enum run_result : int
    {
        RUN_ERROR  = -1,
        OK         = 0,
        NO_EVENTS  = 1
    };

public:
    loop() = default;
    virtual ~loop() = default;

    int run();
    virtual bool stop();

public:
    inline static void call_callback( int fd, short what, callback_info *info )
    {
        if ( info && info->fn )
            info->fn( fd, what );
    }



protected:
    virtual bool        make_config();
    virtual bool        make_base();

    virtual event_ptr   make_event
                        (
                            int fd,
                            short what,
                            callback_fn fn,
                            const timeval *tv = nullptr
                        )
    ;

    virtual callback_info   *make_callback_info
                            (
                                callback_fn fn_
                            )
    ;

public:
    virtual bool init() = 0;
};

}; // namespace eventloop;

#endif // LOOP_H
