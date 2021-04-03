/*
 * Copyright (c) 2021 Alexander Napylov.
 * BSD 2-clause license.
 */

#ifndef TYPES_H
#define TYPES_H

#include <memory>
#include <functional>

#include <event2/visibility.h>
#include <event2/event.h>
#include <event2/listener.h>

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


typedef
    std::unique_ptr<evconnlistener, std::function<void(evconnlistener*)> >
    evconnlistener_ptr
;


};

#endif // TYPES_H
