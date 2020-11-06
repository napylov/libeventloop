#ifndef LOOP_H
#define LOOP_H

#include "types.h"

namespace eventloop
{

class loop
{
private:
    event_cfg_ptr       cfg;
    event_base_ptr      base;

public:
    loop() = default;
    virtual ~loop() = default;


private:
    static event_cfg_ptr    make_config();
    static event_base_ptr   make_base();

    static event_ptr        make_event(
                                event_base_ptr &base,
                                int fd,
                                short what,
                                void *arg,
                                std::function<void(int, short, void*)> fn
                            )
    ;
}

}; // namespace eventloop;

#endif // LOOP_H
