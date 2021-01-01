#include "test_loop.h"
#include "eventloop/custom_arg_base.h"

using namespace eventloop;

bool test_loop::test_make_config()
{
    return make_config();
}


bool test_loop::test_make_base()
{
    return make_base();
}


bool test_loop::test_make_event()
{


    static const int                TEST_FD     = 0;
    static const short              TEST_FLAG   = EV_READ;

    bool callback_has_been_called = false;
    auto test_fn     = [&] ( int fd, short flag )
                            {
                                callback_has_been_called =
                                        fd == TEST_FD &&
                                        flag == TEST_FLAG
                                ;
                            };

    eventloop::event_ptr ev = make_event( TEST_FD, TEST_FLAG, test_fn );
    if ( !ev )
        return false;

    /*
    bool result =
            event_get_fd( ev.get() ) == TEST_FD         &&
            event_get_events( ev.get() ) == TEST_FLAG
    ;

    if ( !result )
        return false;
    */

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


bool test_loop::init()
{
    return make_config() && make_base();
}


TEST_F( test_loop, test1 )
{
    ASSERT_TRUE( test_make_config() );
    ASSERT_TRUE( test_make_base() );
    ASSERT_TRUE( test_make_event() );
}
