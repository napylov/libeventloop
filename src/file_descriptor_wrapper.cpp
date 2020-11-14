#include <unistd.h>
#include "eventloop/file_descriptor_wrapper.h"

namespace eventloop
{


file_descriptor_wrapper::file_descriptor_wrapper()
: fd( -1 )
{
}


file_descriptor_wrapper::~file_descriptor_wrapper()
{
    close();
}


void file_descriptor_wrapper::close()
{
    if ( fd >= 0 )
    {
        ::close( fd );
        fd = -1;
    }
}


ssize_t file_descriptor_wrapper::read( void *buf, size_t size )
{
    return ::read( fd, buf, size );
}


ssize_t file_descriptor_wrapper::write( void *buf, size_t size )
{
    return ::write( fd, buf, size );
}



}
