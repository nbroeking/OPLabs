#ifndef SOCKETADDRESS_HPP_
#define SOCKETADDRESS_HPP_

/*
 * Author: jrahm
 * created: 2015/01/17
 * SocketAddress.hpp: <description>
 */

#include <arpa/inet.h>
#include <prelude.hpp>

namespace io {

class UnknownSocketFamilyException : public Exception {
public:
    UnknownSocketFamilyException(const char* msg): Exception(msg) {};
};

class SocketAddress {
public:
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
};

}

#endif /* SOCKETADDRESS_HPP_ */