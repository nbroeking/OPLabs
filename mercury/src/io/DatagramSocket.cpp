#include <io/DatagramSocket.hpp>

#include <unistd.h>
#include <errno.h>

namespace io {

DatagramSocket::DatagramSocket() {
    m_closed = true;
}

void DatagramSocket::bind( const SocketAddress& addr ) {
    if(!m_closed) close();

    m_fd = socket(addr.linkProtocol(), SOCK_DGRAM, 0);

    if( m_fd < 0 ) {
        throw DatagramSocketException("Unable to create socket", m_fd);
    }

    if( ::bind(m_fd, addr.raw(), addr.rawlen()) ) {
        throw DatagramSocketException("Unable to bind to address", m_fd);
    }

    m_closed = false;
}

void DatagramSocket::close() {
    int rc;
    if( (rc = ::close(m_fd)) ) {
        throw DatagramSocketException("Error on close", rc);
    }

    m_closed = true;
}

ssize_t DatagramSocket::sendTo( const byte* bytes, size_t len, const SocketAddress& to, int flags ) {
    ssize_t ret;
    ret = ::sendto( m_fd, bytes, len, flags, to.raw(), to.rawlen() );
    if(ret < 0) {
        char err[1024];
        snprintf(err, sizeof(err), "sendTo error %s", strerror(errno));
        throw DatagramSocketException(err, ret);
    }
    return ret;
}

ssize_t DatagramSocket::receive( byte* bytes, size_t len, SocketAddress*& faddr, int flags ) {
    byte address_bytes[4096];
    struct sockaddr* addr = (struct sockaddr*)address_bytes;
    socklen_t slen = 0;
    ssize_t ret = ::recvfrom( m_fd, bytes, len, flags, addr, &slen );
    faddr = SocketAddress::toSocketAddress(addr, slen);
    return ret;
}

DatagramSocket::~DatagramSocket() {
    close();
}

}
