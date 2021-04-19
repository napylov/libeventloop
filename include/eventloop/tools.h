#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace eventloop
{


class tools
{
public:
    static std::string get_ip_str( const struct sockaddr *sa )
    {
        static const int BUF_SIZE = 255;
        char s[ BUF_SIZE + 1 ] = { 0 };
        switch(sa->sa_family) {
            case AF_INET:
                inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
                        s, BUF_SIZE);
                break;

            case AF_INET6:
                inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
                        s, BUF_SIZE);
                break;

            default:
                break;
        }

        return std::string(s);
    }
};

}


#endif // TOOLS_H
