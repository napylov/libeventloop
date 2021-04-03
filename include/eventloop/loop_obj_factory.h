/*
 * Copyright (c) 2021 Alexander Napylov.
 * BSD 2-clause license.
 */


#ifndef LOOP_OBJ_FACTORY_H
#define LOOP_OBJ_FACTORY_H


#include "loop.h"


namespace eventloop
{


/**
 * @brief The loop_obj_factory class    Factory to create loop objects.
 */
class loop_obj_factory
{
public:
    /**
     * @brief   make            Make object of class derived from loop.
     * @tparam  obj_type        Making type. It must be derived from loop.
     * @tparam  arg_types       Arguments of obj_type constructor.
     * @param   run_immediate   If true method run() will be called
     *                          for constructed object.
     * @param   args            Arguments of obj_type constructor.
     * @return                  Pointer (unique_ptr) to constructed object.
     */
    template<typename obj_type, typename ... arg_types>
    static
    std::unique_ptr<obj_type>
    make( bool run_immediate, arg_types ... args )
    {
        std::unique_ptr<obj_type> obj = std::make_unique<obj_type>( args ... );
        if ( !obj->init() )
        {
            LOG_ERROR( "Isn't inited." );
            obj.reset();
        }
        else if ( run_immediate )
        {
            LOG_DEBUG( "Run immediate." );
            if ( obj->run() != obj_type::run_result::OK )
            {
                LOG_ERROR( "Run failed" );
                obj.reset();
            }
        }

        return obj;
    }
};


}


#endif // LOOP_OBJ_FACTORY_H
