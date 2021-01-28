#include "test_loop.h"
#include "eventloop/loop_obj_factory.h"

using namespace eventloop;


class test_loop_run : public ::testing::Test
{
public:
    std::unique_ptr<test_loop<void*> >  tested_class;
public:
    test_loop_run() = default;

    virtual void SetUp() override
    {
        tested_class = loop_obj_factory::make<test_loop<void*> >( false );
    }


    virtual void TearDown() override
    {
        tested_class.reset();
    }
};

TEST_F( test_loop_run, test1 )
{
    ASSERT_TRUE( tested_class->test_make_config() );
    ASSERT_TRUE( tested_class->test_make_base() );
    ASSERT_TRUE( tested_class->test_make_event() );
}
