#ifndef INET6ADDRESS_HPP_
#define INET6ADDRESS_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * Inet6Address.hpp: <description>
 */

#include <prelude.hpp>
#include <io/SocketAddressTempl.hpp>

namespace io {

class Inet6ParseException : public Exception {
public:
    Inet6ParseException( const char* message ) : Exception(message){}
};

class Inet6Address : public SocketAddressTempl<sockaddr_in6, AF_INET6> {
public:
    Inet6Address(u8_t addr[16], u16_t port);

    static Inet6Address fromString(const char* ipv6, u16_t port);

    inline Inet6Address(const sockaddr_in6& sockaddr) {
        m_addr = sockaddr;
    }

    std::string toString() const;
};

}

#endif /* INET6ADDRESS_HPP_ */
