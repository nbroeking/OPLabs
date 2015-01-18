#ifndef SOCKETADDRESS_HPP_
#define SOCKETADDRESS_HPP_

/*
 * Author: jrahm
 * created: 2015/01/17
 * SocketAddress.hpp: <description>
 */

#include <arpa/inet.h>

namespace io {

class SocketAddress {
public:
    /**
     * The raw c-style sockaddr that the base class represents
     */
    virtual struct sockaddr* raw() const = 0;

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
