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

    virtual bool init() = 0;
};

}; // namespace eventloop;

#endif // LOOP_H
