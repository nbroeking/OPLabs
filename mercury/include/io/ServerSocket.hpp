#ifndef SERVERSOCKET_HPP_
#define SERVERSOCKET_HPP_

/*
 * Author: jrahm
 * created: 2014/11/07
 * ServerSocket.hpp: <description>
 */

#include <io/Socket.hpp>
#include <netinet/in.h>
#include <prelude.hpp>

namespace io {

class ConnectionException : public CException {
protected:
    inline ConnectionException(const char* message, int rc) :
        CException(message, rc){}

    inline ConnectionException(int rc) :
        CException(rc){}
};

class BindException : public ConnectionException {
public:
    inline BindException( const char* msg, int rc ) : ConnectionException(msg, rc) {}
};

class ListenException : public ConnectionException {
public:
    inline ListenException( const char* msg, int rc ) : ConnectionException(msg, rc) {}
};

/**
 * @brief An abstraction of a server socket over C style Berekly sockets.
 */
class StreamServerSocket: public HasRawFd {
public:
    /**
     * @brief Do all of the work for you. Binds and begins listening.
     * 
     * @param port The port to listen on
     * @param bind_ip The bind ip of the server. Defaults to INADDR_ANY
     * @param backlog The max number of waiting connections
     * 
     * @return The server socket created by this function on success, on error,
     *          NULL is returned
     *
     */
    static StreamServerSocket* createAndBindListen(
						        const SocketAddress& sockaddr,
						        uint32_t backlog = 10) ;

    inline StreamServerSocket() :
        m_fd(-1) {}

    /**
     * @brief Returns the stream socket accepted by the connection
     * @return The new client socket
     */
    StreamSocket* accept() ;

    /**
     * @brief Like the above, but allows for use with non-ipv4 protocols
     * 
     * @param sockaddr the socket address to bind to
     * 
     * @return An error code or 0 if no error
     */
    void bind( const SocketAddress& sockaddr );

    /**
     * @brief begins listening on the port
     * 
     * @param backlog the max number of connections allowed
     * 
     * @return 0 on success, otherwise an error code
     */
    void listen( int backlog = 10 ) ;

    /**
     * @brief Close the file descriptor
     * @return 0 on success, otherwise on error code
     */
    void close();

    inline int getRawFd() { return m_fd; }

    virtual ~StreamServerSocket();   
private:
    int m_fd ;
};

}

#endif /* SERVERSOCKET_HPP_ */
