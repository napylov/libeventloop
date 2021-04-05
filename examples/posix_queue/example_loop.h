#ifndef EXAMPLE_LOOP_H
#define EXAMPLE_LOOP_H

#include <cstddef>
#include "eventloop/loop.h"

using namespace eventloop;

typedef std::nullptr_t example_custom_data_t;

class example_loop : public loop<example_custom_data_t>
{
private:
    int         posix_queue_fd;
    event_ptr   posix_queue_event;

    int         signal_fd;
    event_ptr   signal_event;

public:
    example_loop() = default;
    virtual ~example_loop();

    virtual bool init() override;

    void on_queue(int fd, int flags, const example_custom_data_t &);
    void on_signal(int fd, int flags, const example_custom_data_t &);
};

#endif // EXAMPLE_LOOP_H
