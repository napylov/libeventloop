#include <future>
#include "test_event_queue.h"

void test_event_queue::SetUp()
{
    queue.reset( new eventloop::event_queue<int> );
}


void test_event_queue::TearDown()
{
    queue.reset();
}


TEST_F( test_event_queue, test_push_pop )
{
    static const size_t ARR_SIZE = 3;
    std::array< int, ARR_SIZE > test_data{ 1, 100, -1 };

    size_t i = 0;
    for ( ; i < ARR_SIZE; i++ )
        queue->push( std::move( test_data[ i ] ) );

    for ( i = 0; i < ARR_SIZE; i++ )
        ASSERT_EQ( queue->pop(), test_data[ i ] );
}


TEST_F( test_event_queue, test_pop_delay_if_queue_is_empty )
{
    static const int DELAY = 3;
    static const int TEST_VALUE = 100;

    time_t start_time = time( nullptr );
    std::async
    (
        std::launch::async,
        [&] ()  {
                    sleep( DELAY );
                    queue->push( std::move( TEST_VALUE ) );
                }
    );

    int value = queue->pop();
    time_t diff_time = time( nullptr ) - start_time;

    ASSERT_EQ( value, TEST_VALUE );
    ASSERT_TRUE( diff_time >= DELAY - 1 && diff_time <= DELAY + 1 );
}
