#ifndef FD_FACTORY_H
#define FD_FACTORY_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string>
#include <memory>
#include <functional>

class fd_factory
{
public:
    static const int INVALID_FD = -1;

    typedef std::unique_ptr< addrinfo, std::function<void(addrinfo*)> >
        addrinfo_ptr
    ;

public:
    static addrinfo_ptr get_addrinfo( const char *host, int proto );

    static int connect( const char *host, uint16_t port, int proto );

    static inline int connect( const std::string &host, uint16_t port, int proto )
    {
        return connect( host.c_str(), port, proto );
    }
};

#endif // FD_FACTORY_H
