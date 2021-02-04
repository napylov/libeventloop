#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

#include <chrono>
#include <thread>

#include "eventloop/log.h"


namespace eventloop
{


std::string                          log::fname_pattern;
thread_local std::unique_ptr<log>    log::instance;



bool log::open_log( const char *filename )
{
    bool result = false;
    try
    {
        if ( !instance )
            instance = std::make_unique<log>();
        result = instance->open( filename );
    }
    catch ( ... )
    {
        result = false;
    }

    return result;
}


bool log::open_log( const std::string &filename )
{
    return open_log( filename.c_str() );
}


void log::init( const char *filename_pattern )
{
    fname_pattern = filename_pattern;
}


void log::init( const std::string &filename_pattern )
{
    fname_pattern = filename_pattern;
}


bool log::open( const char *filename )
{
    if ( fd != DEFAULT_FD )
        return false;

    rotate( filename );

    if ( (fd = ::open( filename, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE)) < 0 )
    {
        fd = DEFAULT_FD;
        return false;
    }

    fname = filename;
    return true;
}


void log::close()
{
    if ( fd != DEFAULT_FD )
    {
        ::close( fd );
        rotate( fname.c_str() );
        fd = DEFAULT_FD;
        fname.clear();
    }
}

void log::print( log::LEVEL level, const char *msg, ... )
{
    const std::unique_ptr<log> &inst = get_instance();
    if ( !inst || inst->fd < 0 )
        return;

    if ( level > inst->current_level )
        return;

    va_list arg_list;
    va_start( arg_list, msg );

    static const int BUF_SIZE = 510;
    char buf[ BUF_SIZE + 2 ]; // for \n and \0.

    size_t written = init_out_string( buf, BUF_SIZE, level );
    if ( !written )
        return;

    int tmp = vsnprintf( buf + written, BUF_SIZE - written, msg, arg_list );
    if ( tmp < 0 )
        return;

    static const char STR_END[] = { '\n', '\0' };
    memcpy( buf + written + tmp, STR_END, sizeof( STR_END ) );

    write( inst->fd, buf, written + static_cast<size_t>(tmp) + sizeof( STR_END ) );
}

size_t log::init_out_string( char *buf, size_t buf_size, LEVEL level )
{
    auto now = std::chrono::system_clock::now();
    int64_t mks = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();

    static const int DELIMITER = 1000000;
    tm timeinfo = {0};
    time_t t = mks / DELIMITER;

    localtime_r( &t, &timeinfo );
    size_t size = strftime(buf, buf_size, "%F %T.", &timeinfo);
    if (!size)
        return 0;

    int tmp =
            sprintf(
                buf + size,
                "%06ld %s ",
                static_cast<long>( mks % DELIMITER),
                LEVEL_LABELS[level]
            )
    ;
    if ( tmp < 0 )
        return 0;

    return size + static_cast<size_t>( tmp );
}


void log::rotate( const char *filename )
{
    // TODO: сделать
}


const std::unique_ptr<log> &log::get_instance()
{
    try
    {
        if ( !instance )
        {
            instance.reset( new log );
            if ( !fname_pattern.empty() )
                instance->open( fname_pattern + std::to_string( pthread_self() ) + ".log" );
        }
    }
    catch ( ... )
    {
    }

    return instance;
}


} // namespace eventloop
