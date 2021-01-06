#include "test_loop_obj_factory.h"
#include "eventloop/loop_obj_factory.h"


class loop_for_test_factory : public eventloop::loop
{
public:
    // For test_loop_obj_factory.
    int arg0;
    char arg1;
public:
    loop_for_test_factory() : eventloop::loop(), arg0( 0 ), arg1( 0 ) {}
    loop_for_test_factory( int arg0_, char arg1_ )
         : eventloop::loop(), arg0( arg0_ ), arg1( arg1_ ) {}
    virtual ~loop_for_test_factory() {}

    virtual bool init() override { return true; }
};




test_loop_obj_factory::test_loop_obj_factory()
{
}


TEST_F( test_loop_obj_factory, test_make )
{
    auto loop_ptr = loop_obj_factory::make<loop_for_test_factory>();
    ASSERT_TRUE( loop_ptr.operator bool() );
}


TEST_F( test_loop_obj_factory, test_make_with_args )
{
    static const int TEST_INT = 100;
    static const char TEST_CHAR = 'H';

    auto loop_ptr = loop_obj_factory::make<loop_for_test_factory>( TEST_INT, TEST_CHAR );
    ASSERT_TRUE( loop_ptr.operator bool() );
    ASSERT_TRUE( loop_ptr->arg0 == TEST_INT );
    ASSERT_TRUE( loop_ptr->arg1 == TEST_CHAR );
}

