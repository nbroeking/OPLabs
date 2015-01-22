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
}

int ICMPSocket::init() {
    /* create the socket to use */
    m_fd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);

    /* on an error return the error coed */
    if( m_fd < 0 ) return m_fd ;
    return 0;
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

int ICMPSocket::receive( ICMPPacket& pck, SocketAddress*& into ) {
    byte buffer[1024]; // TODO make this better

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    ssize_t bytes_read;

    if( (bytes_read = recvfrom(m_fd, buffer, sizeof(buffer), 0, (sockaddr*)&addr, &len)) > 0 ) {
        pck.deserialize(buffer + ICMPHEADER_SIZE, bytes_read - ICMPHEADER_SIZE);
        return 0 ;
    }
    
    into = SocketAddress::toSocketAddress((sockaddr*)&addr, len);
    return errno;
}

ssize_t ICMPSocket::send(const ICMPPacket& pkt, const SocketAddress& to_addr) {
    byte* dst ;
    size_t len  = pkt.serializeSize();
    dst = new byte[len];
    pkt.serialize(dst, len);
    
    return sendto( m_fd, dst, len, 0, to_addr.raw(), to_addr.rawlen() );
}

void ICMPSocket::setTimeout( os::micros_t mics ) {
    struct timeval tv = os::Time::toTimeval(mics);
    setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, (byte*)&tv, sizeof(struct timeval));
}


}
