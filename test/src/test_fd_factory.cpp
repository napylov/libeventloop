#include <unistd.h>
#include <sys/signalfd.h>

#include "test_fd_factory.h"
#include "eventloop/fd_factory.h"

bool test_fd_factory::test_create_signal_fd()
{
    std::array<int, 2> sig_arr{ SIGUSR1, SIGUSR2 };
    int fd = eventloop::fd_factory::create_signal_fd( sig_arr );
    if ( fd < 0 )
    {
        std::cout << "Signal fd isn't created.\n";
        return false;
    }

    pid_t pid = getpid();

    int i = 0;
    for ( auto &it : sig_arr )
    {
        if ( !check_signal( fd, pid, it, ++i ) )
            return false;
    }

    return true;
}


bool test_fd_factory::check_signal( int fd, pid_t pid, int signo, int custom )
{
    std::cout << __PRETTY_FUNCTION__ << "\n ";
    sigval sv = { 0 };
    sv.sival_int = custom;

    std::cout << "Send signal " << signo << " with custom data " << custom << "\n";

    sigqueue( pid, signo, sv );
    usleep( 100000 );

    std::cout << "Try read.\n";

    signalfd_siginfo si = { 0 };
    ssize_t readed = read( fd, &si, sizeof(si) );

    std::cout << "readed " << readed << "\n";

    if ( readed < (ssize_t)sizeof(si) )
        return false;

    return si.ssi_signo == static_cast<uint32_t>( signo ) && si.ssi_int == custom;
}


TEST_F( test_fd_factory, test_create_signal_fd )
{
    ASSERT_TRUE( test_create_signal_fd() );
}
