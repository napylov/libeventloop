#include "example_custom_data.h"
#include "eventloop/tools.h"


example_custom_data::example_custom_data()
: fd( -1 ), address()
{
}


example_custom_data::example_custom_data
(
        int                     fd_,
        const struct sockaddr   *addr
) : fd( fd_ ), address( eventloop::tools::get_ip_str( addr ) )
{
}


int example_custom_data::get_fd() const
{
    return fd;
}


const std::string &example_custom_data::get_address() const
{
    return address;
}


