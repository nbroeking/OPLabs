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

/* Constructs a server */
class StreamServerSocket {
public:
    /* Do all of the things */
    static StreamServerSocket* createAndBindListen(
						        uint16_t port,
						        uint32_t bind_ip = INADDR_ANY,
						        uint32_t backlog = 10) ;

    inline StreamServerSocket() :
        m_fd(-1) {}

    /* Creats and returns a stream socket */
    StreamSocket* accept() ;

    /* binds to a port */
    int bind( uint16_t port, uint32_t bind_ip = INADDR_ANY );

    /* initiates the listening */
    int listen( int backlog = 10 ) ;

    /* close the file descriptor */
    int close();

    ~StreamServerSocket();   
private:
    int makeFd();
    int m_fd ;
};

}

#endif /* SERVERSOCKET_HPP_ */
