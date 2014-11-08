#include <io/ServerSocket.hpp>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#include <errno.h>

namespace io {

int StreamServerSocket::makeFd() {
    return socket( AF_INET, SOCK_STREAM, 0 );
}

int StreamServerSocket::bind( uint16_t port, uint32_t bind_ip ) {
    if ( m_fd < 0 ) m_fd = makeFd();
    if ( m_fd < 0 ) return errno ;

    struct sockaddr_in serv_addr;
    memset( &serv_addr, 0, sizeof(serv_addr) );

    serv_addr.sin_family = AF_INET ;
    serv_addr.sin_addr.s_addr = htonl( bind_ip );
    serv_addr.sin_port = htons( port );

    return ::bind( m_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) );
}

int StreamServerSocket::listen( int backlog ) {
    return ::listen( m_fd, backlog );
}

StreamSocket* StreamServerSocket::accept() {
    int connfd = ::accept(m_fd, NULL, NULL);
    if ( connfd <= 0 ) return NULL;
    return new StreamSocket( connfd, true );
}

}
