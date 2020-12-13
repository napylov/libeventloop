#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string>

namespace eventloop
{


enum log_level
{
    ERROR = 1,
    WARNING,
    INFO,
    VERBOSE,
    DEBUG,
    TRACE
};


inline void print_log( log_level level, const char *msg )
{
    // TODO: replace to good logger after debug
    printf( msg );
}


inline void print_log( log_level level, const std::string &msg )
{
    // TODO: replace to good logger after debug
    printf( msg.c_str() );
}



#define log_error( ... )    print_log( log_level::ERROR, __VA_ARGS__ )
#define log_warning( ... )  print_log( log_level::WARNING, __VA_ARGS__ )
#define log_info( ... )     print_log( log_level::INFO, __VA_ARGS__ )
#define log_verbose( ... )  print_log( log_level::VERBOSE, __VA_ARGS__ )
#define log_debug( ... )    print_log( log_level::DEBUG, __VA_ARGS__ )
#define log_trace( ... )    print_log( log_level::TRACE, __VA_ARGS__ )

#define FUNC                { log_trace( __func__ ); log_trace( "\n" ); }

}


#endif // LOG_H
