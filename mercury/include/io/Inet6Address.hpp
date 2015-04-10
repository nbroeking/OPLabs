#ifndef INET6ADDRESS_HPP_
#define INET6ADDRESS_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * Inet6Address.hpp: <description>
 */

#include <Prelude>
#include <io/SocketAddressTempl.hpp>
#include <netinet/in.h>

namespace io {

class Inet6ParseException : public Exception {
public:
    Inet6ParseException( const char* message ) : Exception(message){}
};

/**
 * @brief socket address that describes an IPv6 address
 */
class Inet6Address : public SocketAddressTempl<sockaddr_in6, AF_INET6>, public HasPort {
public:
    /**
     * @brief Create a new ip6 address using the 16 bytes for the address
     * @param addr 16 bytes for the address
     * @param port The port to of the service
     */
    Inet6Address(u8_t addr[16], u16_t port);

    inline Inet6Address() {};

    /**
     * @brief Create an ip6 address from a string and a port
     * @param ipv6 the string to parse
     * @param port the port of the service
     * @return The Inet6Address for the string and the port
     * @throw Inet6ParseException if the string is invalid
     */
    static Inet6Address fromString(const char* ipv6, u16_t port);

    /**
     * @brief Copy constructor
     */
    inline Inet6Address(const sockaddr_in6& sockaddr) {
        m_addr = sockaddr;
    }

    /**
     * @brief Return string representation of this address
     * @return string representation of this address
     */
    std::string toString() const;

    void setPort(u16_t port);
    u16_t getPort();
};

}

#endif /* INET6ADDRESS_HPP_ */
