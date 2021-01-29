#ifndef TEST_SERVER_LOOP_H
#define TEST_SERVER_LOOP_H

#include <list>
#include <gtest/gtest.h>
#include "eventloop/server_loop.h"

using namespace eventloop;

typedef std::nullptr_t tsl_custom;

class test_server_loop : public server_loop<tsl_custom, event_queue_item<tsl_custom> >
{
protected:
    event_ptr   signal_event;
public:
    std::vector<std::string>  input_strings;
    std::vector<std::string>  output_strings;

    int                     fd_ipv4;
    int                     fd_ipv6;

public:
    test_server_loop();
    virtual ~test_server_loop();

    virtual void process_event( event_queue_item<tsl_custom> &&item ) override;

    bool test_connect_ipv4();
    bool test_connect_ipv6();
    bool test_on_client();

    virtual bool init_custom_events() override;
protected:
    void on_signal( int fd, int what );
};

#endif // TEST_SERVER_LOOP_H
