#ifndef TEST_EVENT_QUEUE_H
#define TEST_EVENT_QUEUE_H

#include <gtest/gtest.h>
#include <memory>
#include "eventloop/event_queue.h"

class test_event_queue : public ::testing::Test
{
public:
    std::unique_ptr< eventloop::event_queue<int> >  queue;

public:
    test_event_queue() = default;

    virtual void SetUp() override;
    virtual void TearDown() override;
};

#endif // TEST_EVENT_QUEUE_H
