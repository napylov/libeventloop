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
    typedef std::function<void(int, short, const custom_data_t&)>   callback_fn;
    typedef std::function<void(int)>                                fd_close_fn;

    /**
     * @brief   The event_info struct.
     *          Store information about callback function and custom data.
     */
    struct event_info
    {
        /// Callback function.
        callback_fn     fn;

        /// Custom data.
        custom_data_t   data;

        /// File descriptor close function.
        fd_close_fn     fd_closer;

        event_info() = delete;
        event_info(
                callback_fn fn_,
                custom_data_t &&data_,
                fd_close_fn fd_closer_ = &::close
        )
            : fn( fn_ ), data( data_ ), fd_closer( fd_closer_ )
        {
        }

        event_info(
                callback_fn fn_,
                const custom_data_t &data_,
                fd_close_fn fd_closer_ = &::close
        )
            : fn( fn_ ), data( data_ )
        {
        }

        virtual ~event_info() = default;
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
     * @param arg               Pointer to event_info.
     */
    static void event_callback( int fd, short flags, void *arg )
    {
        call_callback
            (
                fd,
                flags,
                reinterpret_cast<eventloop::loop<custom_data_t>::event_info*>( arg )
            )
        ;
    }


    /**
     * @brief call_callback     Call callback function defined in event_info object.
     * @param fd                File desсriptor.
     * @param what              Flags (see libevent manual).
     * @param info              Info about callback.
     */
    inline static void call_callback
    (
            int fd,
            short what,
            event_info *info
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
                            callback_fn fn,
                            const timeval *tv = nullptr,
                            custom_data_t &&data = custom_data_t(),
                            fd_close_fn fd_closer = &::close
                        )
    {
        FUNC;

        loop::event_info *info = make_event_info( fn, std::move(data), fd_closer );
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
                            const custom_data_t &data = custom_data_t(),
                            fd_close_fn fd_closer = &::close
                        )
    {
        FUNC;

        loop::event_info *info = make_event_info( fn, data, fd_closer );
        if ( !info )
            return event_ptr();

        event_ptr result
                    (
                        event_new( base.get(), fd, what, &event_callback, info ),
                        &event_destructor
                    )
        ;

        if ( result )
        {
            if ( event_add( result.get(), tv ) < 0 )
            {
                LOG_ERROR( "event hasn't been add" );
                result.reset();
            }
        }

        return result;
    }


    virtual event_info   *make_event_info
                            (
                                callback_fn fn,
                                custom_data_t &&data,
                                fd_close_fn fd_closer = &::close
                            )
    {
        FUNC;

        loop::event_info *info = nullptr;
        try
        {
            info = new event_info( fn, std::move( data ), fd_closer );
        }
        catch ( std::bad_alloc & )
        {
        }

        return info;
    }



    virtual event_info   *make_event_info
                            (
                                callback_fn fn,
                                const custom_data_t &data,
                                fd_close_fn fd_closer = &::close
                            )
    {
        FUNC;

        loop::event_info *info = nullptr;
        try
        {
            info = new event_info( fn, data, fd_closer );
        }
        catch ( std::bad_alloc &e )
        {
            LOG_ERROR( e.what() );
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
        FUNC;

        if ( !ev )
            return;

        void *arg = event_get_callback_arg( ev );
        if ( arg )
        {
            event_info *info = reinterpret_cast<event_info*>( arg );
            if ( info && info->fd_closer )
            {
                LOG_DEBUG( "fd_closer is defined. fd %d", event_get_fd( ev ) );
                info->fd_closer( event_get_fd( ev ) );
            }
            delete info;
        }

        event_del( ev );
        event_free( ev );
    }


public:
    /**
     * @brief init  Function to initialize config, base and custom objects.
     *              It called from loop_obj_factory::make().
     *              The function must be implement in derived class to
     *              call make_base() and make_config() (optional).
     * @return      true if success.
     */
    virtual bool init() = 0;
};

}; // namespace eventloop;

#endif // LOOP_H
