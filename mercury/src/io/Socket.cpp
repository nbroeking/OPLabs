#include <io/Socket.hpp>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>

using namespace std;

namespace io {

ssize_t StreamSocket::read( byte* out, size_t len ) {
    return ::read( m_fd, out, len );
}

ssize_t StreamSocket::write( const byte* in, size_t len ) {
    return ::write( m_fd, in, len );
}

int StreamSocket::close() {
    if( m_is_closed ) return 0;

    int rc;
    rc = ::close(m_fd);
    if ( rc ) {
        return rc;
    } else {
        m_is_closed = true;
        return 0;
    }
}

int StreamSocket::connect( const SocketAddress& addr ) {
    m_fd = socket(addr.linkProtocol(), SOCK_STREAM, 0);
    if( m_fd < 0 ) return -2 ;

    if( ::connect( m_fd, addr.raw(), addr.rawlen() ) ) {
        return -3;
    }

    m_is_closed = false;
    return 0;
}

StreamSocket::~StreamSocket() {
    close();
}

}
