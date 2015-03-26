#include <io/ServerSocket.hpp>

#include <sys/socket.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#include <errno.h>

namespace io {

StreamServerSocket* StreamServerSocket::createAndBindListen(
    const SocketAddress& addr, uint32_t backlog ) {

    StreamServerSocket* ret = new StreamServerSocket();
    try {
        ret->bind( addr );
        ret->listen( backlog );
    } catch (CException& err) {
        delete ret;
        throw err;
    }

    return ret;
}

void StreamServerSocket::bind( const SocketAddress& sockaddr ) {
    if ( m_fd < 0 ) m_fd = socket( sockaddr.linkProtocol(), SOCK_STREAM, 0 );
    if ( m_fd < 0 ) {
        throw CException("Error creating socket", m_fd);
    }
    
	const struct sockaddr* raw = sockaddr.raw();
	socklen_t socklen = sockaddr.rawlen();
    int rc = ::bind( m_fd, raw, socklen );
    if( rc ) {
        throw BindException("Error on bind", errno);
    }
}

void StreamServerSocket::listen( int backlog ) {
    int rc = ::listen( m_fd, backlog );
    if ( rc ) {
        throw new ListenException("Error on listen", rc);
    }
}

StreamSocket* StreamServerSocket::accept() {
    int connfd = ::accept(m_fd, NULL, NULL);
    if ( connfd <= 0 ) return NULL;
    return new StreamSocket( connfd, false );
}

void StreamServerSocket::close() {
    int rc;
    if ( (rc = ::close(m_fd)) ) {
        throw CException( rc );
    }
    m_fd = -1;
}

StreamServerSocket::~StreamServerSocket() {
    if( m_fd != -1 ) close();
}

}
