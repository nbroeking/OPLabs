#include <io/DatagramSocket.hpp>
#include <io/UnixAddress.hpp>

#include <unistd.h>
#include <cstdio>

using namespace io;

int main( int argc, char ** argv ) {
    if( argc < 2 ) {
        printf("Need an arguments\n");
        return 1;
    }

    pid_t pid = getpid();
    char name[1024];
    snprintf(name, sizeof(name), "/tmp/.diag.%d.sock", pid);

    UnixAddress from_addr(name);
    from_addr.unlink();

    UnixAddress to_addr(argv[1]);
    DatagramSocket sock;
    sock.bind(from_addr);

    char buf[1024];
    size_t bytes_read;
    while ( (bytes_read = ::read(0, buf, sizeof(buf))) > 0 ) {
        sock.sendTo( (byte*)buf, bytes_read, to_addr );
    }

    return 0;
}
