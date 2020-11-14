#ifndef FILE_DESCRIPTOR_WRAPPER_H
#define FILE_DESCRIPTOR_WRAPPER_H

#include <string>

namespace eventloop
{


class file_descriptor_wrapper
{
private:
    int     fd;
public:
    file_descriptor_wrapper();
    virtual ~file_descriptor_wrapper();

    virtual void open( const char *resource ) = 0;
    virtual void open( const std::string &resource ) = 0;
    virtual void close();

    virtual ssize_t read( void *buf, size_t size );
    virtual ssize_t write( void *buf, size_t size );
};


}

#endif // FILE_DESCRIPTOR_WRAPPER_H
