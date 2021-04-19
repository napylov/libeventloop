#ifndef EXAMPLE_CUSTOM_DATA_H
#define EXAMPLE_CUSTOM_DATA_H

#include <string>
#include <sys/socket.h>

class example_custom_data
{
private:
    int             fd;
    std::string     address;

public:
    example_custom_data();
    example_custom_data(
            int                     fd,
            const struct sockaddr   *addr
    );
    ~example_custom_data() = default;

    int get_fd() const;
    const std::string &get_address() const;
};

#endif // EXAMPLE_CUSTOM_DATA_H
