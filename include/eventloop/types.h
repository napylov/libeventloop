#ifndef TYPES_H
#define TYPES_H

#include <memory>
#include <event2/event.h>

namespace eventloop
{


typedef
    std::unique_ptr<event_config, std::function<void(event_config*)> >
    event_cfg_ptr
;

typedef
    std::unique_ptr<event_base, std::function<void(event_base*)> >
    event_base_ptr
;

typedef
    std::unique_ptr<event, std::function<void(event*)> >
    event_ptr
;


};

#endif // TYPES_H
