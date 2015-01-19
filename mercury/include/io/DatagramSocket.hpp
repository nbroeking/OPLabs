#ifndef DATAGRAMSOCKET_HPP_
#define DATAGRAMSOCKET_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * DatagramSocket.hpp: <description>
 */

#include <io/SocketAddress.hpp>
#include <io/HasRawFd.hpp>

namespace io {

class DatagramSocketException : public CException {
public:
    DatagramSocketException(const char* msg, int rc):
        CException(msg, rc) {};
};

class DatagramSocket : public HasFdTmpl{
public:
    void bind( const SocketAddress& addr ) ;

    void sendTo( const byte* bytes, size_t len,
                const SocketAddress& to, int flags=0 );

    ssize_t receive( byte* bytes, size_t len,
                SocketAddress*& addr, int flags=0 );

    void close();

    ~DatagramSocket();
};

}

#endif /* DATAGRAMSOCKET_HPP_ */
