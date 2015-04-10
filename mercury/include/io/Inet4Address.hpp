#ifndef INET4ADDRESS_HPP_
#define INET4ADDRESS_HPP_

/*
 * Author: jrahm
 * created: 2015/01/17
 * Inet4Address.hpp: <description>
 */

#include <io/SocketAddressTempl.hpp>
#include <types.h>
#include <prelude.hpp>

#include <netinet/in.h>
#include <sys/socket.h>

namespace io {

class InetParseException : public Exception {
public:
    InetParseException(const std::string& message) : Exception(message) {};
};

/**
 * @brief SocketAddress for IPv4
 */
class Inet4Address : public SocketAddressTempl<sockaddr_in, AF_INET>, public HasPort {
public:
    /**
     * @brief Parse the IP address from a sttring
     * @param str The string to parse
     * @param port The port to use
     * @return A new address representing the string
     */
    static Inet4Address fromString(const char * str, u16_t port); /* Throws InetParseException */

    /**
     * @brief Create an Inet4Address from a 4-byte address and port
     * 
     * @param addr The ip address
     * @param port The port number
     */
    Inet4Address(u32_t addr, u16_t port);
    inline Inet4Address() {
        setPort(0);
        setAddress(0);
    }

    Inet4Address(const char* addr, u16_t port);
    inline Inet4Address(const sockaddr_in& addr) {
        m_addr = addr;
    }

    virtual void setPort( u16_t port );
    virtual u16_t getPort();
    virtual void setAddress( u32_t addr );

    virtual std::string toString() const OVERRIDE;
};

}

#endif /* INET4ADDRESS_HPP_ */
