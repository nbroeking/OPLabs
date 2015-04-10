#ifndef SOCKETADDRESS_HPP_
#define SOCKETADDRESS_HPP_

/*
 * Author: jrahm
 * created: 2015/01/17
 * SocketAddress.hpp: <description>
 */

// #include <arpa/inet.h>
#include <prelude.hpp>
#include <sys/socket.h>

namespace io {

/**
 * @brief thrown when an address cannot be deciphered
 */
class UnknownSocketFamilyException : public Exception {
public:
    UnknownSocketFamilyException(const char* msg): Exception(msg) {};
};

class HasPort {
public:
    virtual void setPort( u16_t port ) = 0;
    virtual u16_t getPort() = 0;
};

class SocketAddress {
public:

    /**
     * @brief Create a socket address from a plain old C socket address
     * 
     * @param addr The C socketaddress
     * @param len The length of the struct
     * 
     * @throw UnknownSocketFamilyException if the socket address family cannot be decoded
     * @return A new socket address allocated on the heap
     */
    static SocketAddress* toSocketAddress( struct sockaddr* addr, socklen_t len );

    /**
     * The raw c-style sockaddr that the base class represents
     */
    virtual const struct sockaddr* raw() const = 0;
    virtual struct sockaddr* raw() = 0;

    /**
     * @brief returns the length of the sockaddr returned by `raw()`
     * @return the length of the sockaddr returned by `raw()`
     */
    virtual socklen_t rawlen() const = 0;

    /**
     * @brief returns the correct layer 3 protocol for this address
     * @return the correct layer 3 protocol for this address
     */
    virtual int linkProtocol() const = 0;

    virtual inline ~SocketAddress() {}

    virtual std::string toString() const = 0 ;
};

}

#endif /* SOCKETADDRESS_HPP_ */
