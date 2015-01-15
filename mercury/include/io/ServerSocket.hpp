#ifndef SERVERSOCKET_HPP_
#define SERVERSOCKET_HPP_

/*
 * Author: jrahm
 * created: 2014/11/07
 * ServerSocket.hpp: <description>
 */

#include <types.h>
#include <io/Socket.hpp>
#include <netinet/in.h>

namespace io {

/**
 * @brief An abstraction of a server socket over C style Berekly sockets.
 */
class StreamServerSocket {
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
						        uint16_t port,
						        uint32_t bind_ip = INADDR_ANY,
						        uint32_t backlog = 10) ;

    inline StreamServerSocket() :
        m_fd(-1) {}

    /**
     * @brief Returns the stream socket accepted by the connection
     * @return The new client socket
     */
    StreamSocket* accept() ;

    /**
     * @brief Binds a server socket to a port and an address
     * 
     * @param port The port to bind to
     * @param bind_ip The ip to bind to
     * 
     * @return 0 on success, otherwise an error code is set
     */
    int bind( uint16_t port, uint32_t bind_ip = INADDR_ANY );

    /**
     * @brief begins listening on the port
     * 
     * @param backlog the max number of connections allowed
     * 
     * @return 0 on success, otherwise an error code
     */
    int listen( int backlog = 10 ) ;

    /**
     * @brief Close the file descriptor
     * @return 0 on success, otherwise on error code
     */
    int close();

    ~StreamServerSocket();   
private:
    int makeFd();
    int m_fd ;
};

}

#endif /* SERVERSOCKET_HPP_ */
