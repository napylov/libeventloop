#ifndef TEST_LOOP_H
#define TEST_LOOP_H

#include <cstddef>
#include <gtest/gtest.h>
#include "eventloop/loop.h"

template <typename T>
class test_loop : public eventloop::loop<T>
{
public:
    // For test_loop_obj_factory.
    int arg0;
    char arg1;
public:
    test_loop()
    :   eventloop::loop<T>(),
        arg0( 0 ),
        arg1( 0 )
    {
    }

    test_loop( int arg0_, char arg1_ )
    :   eventloop::loop<T>(),
        arg0( arg0_ ),
        arg1( arg1_ )
    {
    }


    virtual ~test_loop() = default;

    bool test_make_config()
    {
        return this->make_config();
    }

    bool test_make_base()
    {
        return this->make_base();
    }

    bool test_make_event()
    {
        static const int                TEST_FD     = 0;
        static const short              TEST_FLAG   = EV_READ;

        bool callback_has_been_called = false;
        auto test_fn     = [&] ( int fd, short flag, const T &t )
                                {
                                    callback_has_been_called =
                                            fd == TEST_FD &&
                                            flag == TEST_FLAG
                                    ;
                                };

        /*T t;*/
        eventloop::event_ptr ev =
                this->make_event(
                        TEST_FD,
                        TEST_FLAG,
                        test_fn,
                        nullptr,
                        T()
                )
        ;
        if ( !ev )
            return false;

        event_callback_fn callback = event_get_callback( ev.get() );
        callback
                (
                    event_get_fd( ev.get() ),
                    event_get_events( ev.get() ),
                    event_get_callback_arg( ev.get() )
                )
        ;

        return callback_has_been_called;
    }


    virtual bool init() override
    {
        return this->make_config() && this->make_base();
    }

};


#endif // TEST_LOOP_H
