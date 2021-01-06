#ifndef TEST_LOOP_H
#define TEST_LOOP_H

#include <gtest/gtest.h>
#include "eventloop/loop.h"



class test_loop : public eventloop::loop, public ::testing::Test
{
public:
    // For test_loop_obj_factory.
    int arg0;
    char arg1;
public:
    test_loop();
    test_loop( int arg0_, char arg1_ );
    virtual ~test_loop() = default;

    //virtual void SetUp() override;
    //virtual void TearDown() override;

    bool test_make_config();
    bool test_make_base();
    bool test_make_event();

    virtual bool init() override;
};

#endif // TEST_LOOP_H
