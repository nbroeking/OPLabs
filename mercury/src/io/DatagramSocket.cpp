#include <io/DatagramSocket.hpp>

#include <unistd.h>

namespace io {

void DatagramSocket::bind( const SocketAddress& addr ) {
    m_fd = socket(addr.linkProtocol(), SOCK_DGRAM, 0);

    if( m_fd < 0 ) {
        throw DatagramSocketException("Unable to create socket", m_fd);
    }

    if( ::bind(m_fd, addr.raw(), addr.rawlen()) ) {
        throw DatagramSocketException("Unable to bind to address", m_fd);
    }
}

void DatagramSocket::close() {
    int rc;
    if( (rc = ::close(m_fd)) ) {
        throw DatagramSocketException("Error on close", rc);
    }
}

void DatagramSocket::sendTo( const byte* bytes, size_t len, const SocketAddress& to, int flags ) {
    ::sendto( m_fd, bytes, len, flags, to.raw(), to.rawlen() );
}

ssize_t DatagramSocket::receive( byte* bytes, size_t len, SocketAddress*& faddr, int flags ) {
    byte address_bytes[4096];
    struct sockaddr* addr = (struct sockaddr*)address_bytes;
    socklen_t slen;
    ssize_t ret = ::recvfrom( m_fd, bytes, len, flags, addr, &slen );
    faddr = SocketAddress::toSocketAddress(addr, slen);
    return ret;
}

DatagramSocket::~DatagramSocket() {
    close();
}

}
