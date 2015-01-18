#ifndef INET4ADDRESS_HPP_
#define INET4ADDRESS_HPP_

/*
 * Author: jrahm
 * created: 2015/01/17
 * Inet4Address.hpp: <description>
 */

#include <io/SocketAddress.hpp>
#include <types.h>

#include <prelude.hpp>

namespace io {

class InetParseException : public Exception {
public:
    InetParseException(const std::string& message) : Exception(message) {};
};

class Inet4Address : public SocketAddress {
public:
    static Inet4Address fromString(const char *, u16_t port); /* Throws InetParseException */

    Inet4Address(u32_t addr, u16_t port);
    Inet4Address(const char* addr, u16_t port);
    inline Inet4Address(const sockaddr_in& addr): m_addr(addr){}

    virtual void setPort( u16_t port );
    virtual void setAddress( u32_t addr );

    virtual struct sockaddr* raw() const OVERRIDE;
    virtual socklen_t rawlen() const OVERRIDE;
    virtual int linkProtocol() const OVERRIDE { return AF_INET; };
private:
    struct sockaddr_in m_addr;
};

}

#endif /* INET4ADDRESS_HPP_ */
