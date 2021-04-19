#include <memory>

#include "eventloop/server_loop.h"
#include "eventloop/loop_obj_factory.h"
#include "example_server_loop.h"


int main()
{
    // Create example_loop object and run loop.
    std::unique_ptr<example_server_loop> example =
            loop_obj_factory::make<example_server_loop>( true, 3000 )
    ;

    return 0;
}
