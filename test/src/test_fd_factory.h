#ifndef TEST_FD_FACTORY_H
#define TEST_FD_FACTORY_H

#include <gtest/gtest.h>

class test_fd_factory : public ::testing::Test
{
public:
    test_fd_factory() = default;

    bool test_create_signal_fd();
private:
    bool check_signal( int fd, pid_t pid, int signo, int custom );
};

#endif // TEST_FD_FACTORY_H
