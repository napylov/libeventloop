#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>

#include "example_loop.h"
#include "eventloop/fd_factory.h"


bool example_loop::init()
{
    // Initialize base (pointer to event_base).
    if ( !make_base() )
        return false;

    // Message queue name.
    static const char EXAMPLE_MQ[] = "/eventloop_example";

    // Open queue.
    int posix_queue_fd = mq_open(
                EXAMPLE_MQ,
                O_RDONLY | O_CREAT | O_NONBLOCK,
                0600,
                nullptr
    );
    // If open queue is unsecessful return false.
    if ( posix_queue_fd < 0 )
    {
        LOG_ERROR( "Can't open queue. errno %d", errno );
        return false;
    }

    // Make pointer to event object.
    posix_queue_event =
            make_event(
                posix_queue_fd,             // File descriptor.
                EV_READ | EV_PERSIST,       // Flags.
                std::bind(                  // Bind callback function.
                    &example_loop::on_queue,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3
                ),
                nullptr,                    // Timeval isn't used in the example.
                nullptr,                    // Custom data isn't used in the example.
                [&] ( int fd )              // Function to close file descriptor.
                {
                    ::mq_close( fd );
                    ::mq_unlink( EXAMPLE_MQ );
                }
            )
    ;

    // If unseccesful return false.
    if ( !posix_queue_event )
    {
        LOG_ERROR( "!posix_queue_event" );
        return false;
    }


    // Create file descriptor for signals.
    // Argument - container with signals number.
    int signal_fd = fd_factory::create_signal_fd<std::array<int, 3> >( { SIGTERM, SIGUSR1, SIGUSR2 } );
    if ( signal_fd < 0 )
    {
        LOG_ERROR( "Signal fd hasn't been inited." );
        return false;
    }

    // Make pointer to event object. Used file descripor closer by default (close(int)).
    signal_event =
            make_event(
                signal_fd,
                EV_READ | EV_PERSIST,
                std::bind(
                    &example_loop::on_signal,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3
                ),
                nullptr,
                nullptr
            )
    ;

    return signal_event.operator bool();
}


void example_loop::on_queue(int fd, int, const example_custom_data_t &)
{
    static const char exit[] = "exit";
    char buf[ 65536 ] = { 0 };

    ssize_t readed = mq_receive( fd, buf, sizeof(buf), nullptr );
    std::cout << __func__ << " readed " << readed << " bytes\n";
    if ( readed < 0 )
        return;

    if ( readed == sizeof(exit) - 1 && !strncmp( buf, exit, sizeof (exit) - 1 ) )
    {
        std::cout << "exit\n";
        stop();
    }
}


void example_loop::on_signal(int fd, int, const example_custom_data_t &)
{
    signalfd_siginfo sig_info = { 0 };
    if ( read( fd, &sig_info, sizeof(sig_info) ) <
         static_cast<ssize_t>( sizeof( sig_info ) )
    )
    {
        std::cout << "Can't read signal.\n";
        return;
    }

    std::cout << "Got signal " << sig_info.ssi_signo << "\n";
    if ( sig_info.ssi_signo == SIGINT )
        stop();
}
