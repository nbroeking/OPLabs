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
    ssize_t ret = ::read( m_fd, out, len );
    return ret;
}

ssize_t StreamSocket::write( const byte* in, size_t len ) {
    ssize_t ret = ::send( m_fd, in, len, m_options );
    if ( ret < 0 ) {
        throw CException("Unable to write", ret);
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

	int rc;
    if( m_fd < 0 ) {
		throw ConnectException("Unable to connect", m_fd);
	}

    if( (rc = ::connect( m_fd, addr.raw(), addr.rawlen() )) ) {
		throw ConnectException("Unable to connect", rc);
    }

    m_is_closed = false;
    return 0;
}

StreamSocket::~StreamSocket() {
    close();
}

}
