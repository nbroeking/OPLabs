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

int StreamSocket::connect( const char* hostname, unsigned short port ) {
    struct hostent* host;
    struct sockaddr_in server;

    host = gethostbyname( hostname );
    if( ! host ) return -1;

    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if( m_fd < 0 ) return -2 ;

    copy( (byte*)host->h_addr,
          (byte*)host->h_addr + sizeof( host->h_length ),
          /* Into */
		  (byte*)&server.sin_addr.s_addr );

    server.sin_family = AF_INET;
    server.sin_port = htons( port );

    if( ::connect( m_fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in) ) ){
        return -3;
    }

    m_is_closed = false;
    return 0;
}

StreamSocket::~StreamSocket() {
    close();
}

}
