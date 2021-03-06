#include <memory>

#include "eventloop/loop.h"
#include "eventloop/loop_obj_factory.h"
#include "example_loop.h"


int main()
{
    // Create example_loop object and run loop.
    std::unique_ptr<example_loop> example =
            loop_obj_factory::make<example_loop>( true )
    ;

    return 0;
}
