/*
 * Copyright (c) 2021 Alexander Napylov.
 * BSD 2-clause license.
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string>
#include <memory>

#include <unistd.h>

#ifndef EVENTLOOP_LOG
    #define EVENTLOOP_LOG   1
#endif


#if EVENTLOOP_LOG


namespace eventloop
{


class log
{
public:
    enum LEVEL : uint8_t
    {
        ERROR = 1,
        WARNING,
        INFO,
        VERBOSE,
        DEBUG,
        TRACE
    };

    static constexpr std::array< char[8], 7 > LEVEL_LABELS =
        {
            "       ",
            "  ERROR",
            "WARNING",
            "   INFO",
            "VERBOSE",
            "  DEBUG",
            "  TRACE"
        }
    ;

private:
    static const int    DEFAULT_FD      = STDOUT_FILENO;

    LEVEL               current_level   = LEVEL::TRACE;
    int                 fd              = DEFAULT_FD;

private:
    static std::string                          fname_pattern;
    std::string                                 fname;
    static thread_local std::unique_ptr<log>    instance;

public:
    log() = default;
    ~log()
    {
        close();
    }

    static bool open_log( const char *filename );
    static bool open_log( const std::string &filename );
    static void init( const char *filename_pattern );
    static void init( const std::string &filename_pattern );

    bool open( const char *filename );
    inline bool open( const std::string &filename )
    {
        return open( filename.c_str() );
    }
    void close();

    static void print( LEVEL level, const char *msg, ... );
    inline static void print( LEVEL level, const std::string &msg, ... )
    {
        // TODO: replace to good logger after debug
        printf( msg.c_str() );
    }

private:
    static size_t init_out_string( char *buf, size_t buf_size, LEVEL level );
    void rotate( const char *filename );

    static const std::unique_ptr<log> &get_instance();
};


} // namespace eventloop


#define LOG_ERROR( ... )    log::print( log::LEVEL::ERROR, __VA_ARGS__ )
#define LOG_WARNING( ... )  log::print( log::LEVEL::WARNING, __VA_ARGS__ )
#define LOG_INFO( ... )     log::print( log::LEVEL::INFO, __VA_ARGS__ )
#define LOG_VERBOSE( ... )  log::print( log::LEVEL::VERBOSE, __VA_ARGS__ )
#define LOG_DEBUG( ... )    log::print( log::LEVEL::DEBUG, __VA_ARGS__ )
#define LOG_TRACE( ... )    log::print( log::LEVEL::TRACE, __VA_ARGS__ )

#define FUNC                log::print( log::LEVEL::TRACE, ">>> %s %s:%d", __func__, __FILE__, __LINE__ )

#else // if EVENTLOOP_LOG

#define LOG_ERROR( ... )
#define LOG_WARNING( ... )
#define LOG_INFO( ... )
#define LOG_VERBOSE( ... )
#define LOG_DEBUG( ... )
#define LOG_TRACE( ... )

#define FUNC


#endif // if EVENTLOOP_LOG


#endif // LOG_H
