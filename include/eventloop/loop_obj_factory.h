#ifndef LOOP_OBJ_FACTORY_H
#define LOOP_OBJ_FACTORY_H


#include "loop.h"


namespace eventloop
{


class loop_obj_factory
{
public:
    template<typename obj_type, typename ... arg_types>
    static
    std::unique_ptr
    <
        typename std::enable_if
        <
            std::is_base_of<loop, obj_type>::value,
            obj_type
        >::type
    >
    make( bool run_immediate, arg_types ... args )
    {
        std::unique_ptr<obj_type> obj = std::make_unique<obj_type>( args ... );
        if ( !obj->init() )
            obj.reset();
        else if ( run_immediate )
        {
            if ( obj->run() != loop::run_result::OK )
                obj.reset();
        }

        return obj;
    }
};


}


#endif // LOOP_OBJ_FACTORY_H
