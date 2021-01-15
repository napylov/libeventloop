#ifndef TEST_SERVER_LOOP_H
#define TEST_SERVER_LOOP_H

#include <list>
#include <gtest/gtest.h>
#include "eventloop/server_loop.h"

using namespace eventloop;

class test_server_loop : public server_loop<event_queue_item>
{
public:
    std::list<std::string>  input_strings;
    std::list<std::string>  output_strings;

    int                     fd_ipv4;
    int                     fd_ipv6;

public:
    test_server_loop();
    virtual ~test_server_loop();

    virtual void process_event( event_queue_item &&item ) override;

    bool test_connect_ipv4();
    bool test_connect_ipv6();
    bool test_on_client();
};

#endif // TEST_SERVER_LOOP_H
