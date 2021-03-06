#ifndef EXAMPLE_SERVER_LOOP_H
#define EXAMPLE_SERVER_LOOP_H

#include "eventloop/server_loop.h"
#include "example_custom_data.h"

using namespace eventloop;


typedef std::shared_ptr<example_custom_data>    example_custom_data_ptr;


typedef
    server_loop<
        example_custom_data_ptr,
        event_queue_item<example_custom_data_ptr>
    >
    example_server_loop_base
;


class example_server_loop : public example_server_loop_base
{
private:
    event_ptr   signal_event;

public:
    example_server_loop();
    example_server_loop( uint16_t port );


protected:
    virtual example_custom_data_ptr make_custom_data_on_accept
    (
            evutil_socket_t         fd,
            const struct sockaddr   *addr,
            int                     sock_len
    ) override;

    virtual void process_event( event_queue_item<example_custom_data_ptr> &&item ) override;

    void print_buf( uint8_t *buf, ssize_t size, const std::string &addr );

    void on_signal(int fd, int, const example_custom_data_ptr &);

public:
    virtual bool init_custom_events() override;
};


#endif // EXAMPLE_SERVER_LOOP_H
