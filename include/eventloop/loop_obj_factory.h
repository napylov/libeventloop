#ifndef LOOP_OBJ_FACTORY_H
#define LOOP_OBJ_FACTORY_H


#include "loop.h"


namespace eventloop
{


class loop_obj_factory
{
    template
        <
            typename obj_type,
            std::enable_if<
                std::is_base_of<loop, obj_type>::value,
                obj_type
            >
        >
    obj_type* make()
    {
        obj_type *obj = new obj_type;
        obj->init();
    }


    template
        <
            typename obj_type,
            std::enable_if<
                std::is_base_of<loop, obj_type>::value,
                obj_type
            >,
            typename ... arg_types
        >
    obj_type* make_with_args( arg_types ... args )
    {
        obj_type *obj = new obj_type( args ... );
        obj->init();
    }

};


}


#endif // LOOP_OBJ_FACTORY_H
