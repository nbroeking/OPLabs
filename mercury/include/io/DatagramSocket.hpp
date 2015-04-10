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

/**
 * @brief Exception pertaining to DatagramSockets
 */
class DatagramSocketException : public CException {
public:
    /**
     * @brief Create a new DatagramSocketException
     * @param msg The message to tell the user
     * @param rc The return code from the C library
     */
    DatagramSocketException(const char* msg, int rc):
        CException(msg, rc) {};
};

/**
 * @brief A socket that implements the Datagram protocol (UDP)
 *
 * This socket is closed on destruction
 */
class DatagramSocket : public HasFdTmpl{
public:
    /**
     * @brief Bind this socket to an address
     * @param addr The address to bind to
     */
    void bind( const SocketAddress& addr ) ;

    /**
     * @brief Send some bytes to an address
     * 
     * @param bytes The bytes to send
     * @param len The length of the bytes to send
     * @param to The address of the recipient
     * @param flags Extra C flags (default 0)
     */
    ssize_t sendTo( const byte* bytes, size_t len,
                const SocketAddress& to, int flags=0 );

    /**
     * @brief Block and wait for data on this socket
     * 
     * @param bytes The array of bytes to write to
     * @param len The length of the array of bytes
     * @param addr The reference to store the from address
     * @param flags Extra C flags (default 0)
     * 
     * @return Number of bytes read
     */
    ssize_t receive( byte* bytes, size_t len,
                SocketAddress*& addr, int flags=0 );

    /**
     * @brief Close this socket
     */
    void close();

    ~DatagramSocket();
    DatagramSocket();

private:
    bool m_closed;
};

}

#endif /* DATAGRAMSOCKET_HPP_ */
