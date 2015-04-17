#include <io/Socket.hpp>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include <cerrno>

#include <unistd.h>
#include <fcntl.h>

using namespace std;

namespace io {

ssize_t StreamSocket::read( byte* out, size_t len ) {
    ssize_t ret = ::read( m_fd, out, len );
    return ret;
}

ssize_t StreamSocket::write( const byte* in, size_t len ) {
    ssize_t ret = ::write( m_fd, in, len );

    if ( ret < 0 ) {
        if(errno == EWOULDBLOCK) {
            /* If we would have blocked, then
             * we wrote 0 bytes */
            return 0;
        }
        throw CException("Unable to write", errno);
    }
    return ret;
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
    m_options = fcntl(m_fd, F_GETFL, 0);

    char buf[4096];
	int rc;
    if( m_fd < 0 ) {
        snprintf(buf, sizeof(buf), "StreamSocketException: unable to create socket for %s", 
            addr.toString().c_str());
		throw ConnectException(buf, errno);
	}

    if( (rc = ::connect( m_fd, addr.raw(), addr.rawlen() )) ) {
        snprintf(buf, sizeof(buf), "StreamSocketException: unable to connect to address %s", 
            addr.toString().c_str());
		throw ConnectException(buf, errno);
    }

    m_is_closed = false;
    return 0;
}

void StreamSocket::setOption(int option) {
    m_options |= option;   
    fcntl(m_fd, F_SETFL, m_options);
}

void StreamSocket::unsetOption(int option) {
    m_options &= ~option;
    fcntl(m_fd, F_SETFL, m_options);
}

StreamSocket::~StreamSocket() {
    close();
}

}
