#ifndef EXAMPLE_LOOP_H
#define EXAMPLE_LOOP_H

#include <cstddef>
#include "eventloop/loop.h"

using namespace eventloop;

// Determine nullptr_t as custom data because in the
// examples custom data is not used.
typedef std::nullptr_t example_custom_data_t;


/**
 * @brief The example_loop class    Example event loop.
 */
class example_loop : public loop<example_custom_data_t>
{
private:
    // Pointer to object to process events from POSIX message queue.
    event_ptr   posix_queue_event;

    // Pointer to object to process signal's event.
    event_ptr   signal_event;

public:
    example_loop() = default;
    virtual ~example_loop() = default;

    // Initialize objects.
    virtual bool init() override;

    // Callback for events from message queue.
    void on_queue(int fd, int flags, const example_custom_data_t &);

    // Callback to events from signals.
    void on_signal(int fd, int flags, const example_custom_data_t &);
};

#endif // EXAMPLE_LOOP_H
