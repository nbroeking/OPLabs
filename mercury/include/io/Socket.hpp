#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include "types.h"
#include <cstdio>

#include <io/BaseIO.hpp>

namespace io {

/*
 * Author: jrahm
 * created: 2014/11/07
 * Socket.hpp: <description>
 */

/**
 * @brief An abstraction on top of the standard Berekly sockets to represend a client socket.
 */
class StreamSocket : public BaseIO {
public:

    /**
     * @brief Construct a new socket
     */
    inline StreamSocket() : m_fd(-1), m_is_closed( true ) {} ;

    /**
     * @brief Contstruct a new socket from a c style berekly socket
     * 
     * @param fd The file descriptor to use
     * @param is_closed true if the file descriptor is closed
     *
     * This function will take ownership of the file descriptor,
     * is it is important to make sure all references to it are
     * destroyed
     */
    inline StreamSocket(int fd, bool is_closed) :
        m_fd( fd ), m_is_closed( is_closed ) {}

    
    /**
     * @brief Attempt to connect to a host on a port
     * 
     * @param host the host to connect to (This function does do DNS)
     * @param port the port on the host to connect to 
     * 
     * @return 0 on success, otherwise an error code on failure
     */
    int connect( const char* host, unsigned short port ) ;

    /* Reads from the socket into the byte array given */
    virtual ssize_t read( byte* out, size_t len ) OVERRIDE;

    /* Writes data to the socket. Will write at most `len`
     * bytes to the output. Returns the number of bytes
     * written or -rc on failure. */
    virtual ssize_t write( const byte* data, size_t len ) OVERRIDE;

    /* Shut down the socket */
    int close( ) OVERRIDE ;

    virtual ~StreamSocket();

    /**
     * @brief Tells whether or not the socket is closed
     * @return True if this socket is closed
     */
    inline bool isClosed() {
        return m_is_closed;
    }

private:
    int m_fd;
    bool m_is_closed;
};

/* Class defining a socket address */
class SocketAddress {
public:
    uint32_t host;
    uint16_t port;

    inline SocketAddress( uint32_t host, uint32_t port ) :
        host( host ), port( port ) {}
};

/* Class describing a stateless UDP session */
class DatagramSocket {
public:
    ssize_t read( byte* out, size_t len, SocketAddress& sender );

    ssize_t write( const byte* data, size_t len, const SocketAddress& out );

    int close();
};

}

#endif /* SOCKET_HPP_ */
