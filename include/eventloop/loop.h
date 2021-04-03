/*
 * Copyright (c) 2021 Alexander Napylov.
 * BSD 2-clause license.
 */

#ifndef LOOP_H
#define LOOP_H

#include <functional>
#include <utility>

#include "types.h"
#include "log.h"

namespace eventloop
{

/**
 *  Base class for event loop.
 */
template <typename custom_data_t>
class loop
{
protected:
    event_cfg_ptr               cfg;
    event_base_ptr              base;

public:
    typedef std::function<void(int, short, const custom_data_t&)>  callback_fn;

    /**
     * @brief   The callback_info struct.
     *          Store information about callback function and custom data.
     */
    struct callback_info
    {
        /// Callback function.
        callback_fn     fn;

        /// Custom data.
        custom_data_t   data;

        callback_info() = delete;
        callback_info( callback_fn fn_, custom_data_t &&data_ )
            : fn( fn_ ), data( data_ ) {}
        callback_info( callback_fn fn_, const custom_data_t &data_ )
            : fn( fn_ ), data( data_ ) {}
        virtual ~callback_info() = default;
    };


    /**
     * @brief The run_result enum   Result of eveny loop run.
     *                              The constants in the enum is same
     *                              values returned from event_base_dispatch()
     *                              (see libevent manual).
     */
    enum run_result : int
    {
        RUN_ERROR  = -1,
        OK         = 0,
        NO_EVENTS  = 1
    };

public:
    loop() = default;
    virtual ~loop() = default;

    /**
     * @brief run   Run event loop.
     * @return      Result of run returned from event_base_dispatch(). See libevent manual.
     */
    int run()
    {
        return event_base_dispatch( base.get() );
    }

    /**
     * @brief stop  Stop event loop.
     * @return      True if success.
     */
    virtual bool stop()
    {
        return event_base_loopexit( base.get(), nullptr ) != -1;
    }


public:
    /**
     * @brief event_callback    Callback for event from libevent.
     *                          The function call callback function defined
     *                          in the wrapper's event.
     * @param fd                File descriptor.
     * @param flags             Flags (see libevent manual).
     * @param arg               Pointer to callback_info.
     */
    static void event_callback( int fd, short flags, void *arg )
    {
        call_callback
            (
                fd,
                flags,
                reinterpret_cast<eventloop::loop<custom_data_t>::callback_info*>( arg )
            )
        ;
    }


    /**
     * @brief call_callback     Call callback function defined in callback_info object.
     * @param fd                File desсriptor.
     * @param what              Flags (see libevent manual).
     * @param info              Info about callback.
     */
    inline static void call_callback
    (
            int fd,
            short what,
            callback_info *info
    )
    {
        if ( info && info->fn )
            info->fn( fd, what, info->data );
    }




protected:
    virtual bool        make_config()
    {
        FUNC;

        cfg = event_cfg_ptr( event_config_new(), &config_destructor );
        return cfg.operator bool();
    }

    virtual bool        make_base()
    {
        FUNC;

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


    virtual event_ptr   make_event
                        (
                            int fd,
                            short what,
                            custom_data_t &&data,
                            callback_fn fn,
                            const timeval *tv = nullptr
                        )
    {
        FUNC;

        loop::callback_info *info = make_callback_info( fn, std::move(data) );
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


    virtual event_ptr   make_event
                        (
                            int fd,
                            short what,
                            callback_fn fn,
                            const timeval *tv = nullptr,
                            const custom_data_t &data = custom_data_t()
                        )
    {
        FUNC;

        loop::callback_info *info = make_callback_info( fn, data );
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


    virtual callback_info   *make_callback_info
                            (
                                callback_fn fn,
                                custom_data_t &&data
                            )
    {
        FUNC;

        loop::callback_info *info = nullptr;
        try
        {
            info = new callback_info( fn, std::move( data ) );
        }
        catch ( std::bad_alloc & )
        {
        }

        return info;
    }



    virtual callback_info   *make_callback_info
                            (
                                callback_fn fn,
                                const custom_data_t &data
                            )
    {
        FUNC;

        loop::callback_info *info = nullptr;
        try
        {
            info = new callback_info( fn, data );
        }
        catch ( std::bad_alloc & )
        {
        }

        return info;
    }

public:
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
            callback_info *info = reinterpret_cast<callback_info*>( arg );
            delete info;
        }

        event_del( ev );
        event_free( ev );
    }


public:
    virtual bool init() = 0;
};

}; // namespace eventloop;

#endif // LOOP_H
