#include <io/ICMPSocket.hpp>

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#include <algorithm>

using namespace std ;

namespace io {

ICMPSocket::ICMPSocket() {
    m_fd = -1 ;
    m_seq = 0 ;
    fill( (byte*)&m_packet_header, (byte*)&m_packet_header + sizeof( m_packet_header ), 0 ) ;
}

int ICMPSocket::init() {
    /* create the socket to use */
    m_fd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);

    /* on an error return the error coed */
    if( m_fd < 0 ) return m_fd ;
}

int ICMPSocket::setNonBlocking(bool yes) {
    int opts = fcntl(m_fd, F_GETFL);

    if( yes ) {
        opts |= O_NONBLOCK;
    } else {
        opts &= ~O_NONBLOCK;
    }

    return fcntl(m_fd, F_SETFL, opts);
}

int ICMPSocket::receive( ICMPPacket& pck, struct sockaddr* addr, socklen_t& len ) {
    byte buffer[1024]; // TODO make this better
    ssize_t bytes_read;
    size_t iph_size = sizeof( struct iphdr );
    if( (bytes_read = recvfrom(m_fd, buffer, sizeof(buffer), 0, addr, &len)) > 0 ) {
        pck.deserialize(buffer + iph_size, bytes_read - iph_size);
        return 0 ;
    }
    return errno;
}

ssize_t ICMPSocket::send(const ICMPPacket& pkt, struct sockaddr* addr, socklen_t socklen) {
    byte* dst ;
    size_t len  = pkt.serializeSize();
    dst = new byte[len];
    pkt.serialize(dst, len);
    
    return sendto( m_fd, dst, len, 0, addr, socklen );
}

void ICMPSocket::setTimeout( os::micros_t mics ) {
    struct timeval tv = os::Time::toTimeval(mics);
    setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, (byte*)&tv, sizeof(struct timeval));
}


}
