#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>

#include <missing.h>

#include <io/Inet4Address.hpp>
#include <io/Inet6Address.hpp>
#include <io/UnixAddress.hpp>

namespace io {


Inet4Address Inet4Address::fromString(const char* ip) {
    std::string str = ip;
    size_t itr = str.find(':');
    if(itr == std::string::npos) {
        return io::Inet4Address(str.c_str(), 0);
    } else {
        std::string addr = str.substr(0, itr);
        std::string port = str.substr(itr + 1);
        return io::Inet4Address(addr.c_str(), atoi(port.c_str()));
    }
}

Inet4Address Inet4Address::fromString(const char *str, u16_t port) {
    u32_t addr;
    u32_t buffer = 0;
    u32_t ndots = 0;

    const char* cur = str ;
    for ( ; *cur != '\0' ; ++ cur ) {
        if( *cur == '.' ) {
            if( ndots == 3 )
                throw InetParseException("Too many octects in IPv4 address");

            addr = (addr << 8) + buffer;
            buffer = 0;
            ndots ++;
        } else {
            if( *cur > 0x39 || *cur < 0x30 )
                throw InetParseException("Non-numeric characters in inet address");
    
            buffer = buffer * 10 + (*cur - 0x30);
            if( buffer > 255 )
                throw InetParseException("Octect out of range in inet address");
        }
    }
    addr = (addr << 8) + buffer;

    return Inet4Address(addr, port);
}

Inet4Address::Inet4Address(u32_t addr, u16_t port) {
    m_addr.sin_addr.s_addr = htonl(addr);
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons( port );
}

Inet4Address::Inet4Address(const char* addr, u16_t port) {
    if( inet_aton(addr, &m_addr.sin_addr) != 1 ) {
        char buf[1024];
        snprintf(buf, 1024, "Error parsing Inet4Address %s:%hu", addr, port);
        throw InetParseException(buf);
    }
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
}

u16_t Inet4Address::getPort() {
    return ntohs(m_addr.sin_port);
}

void Inet4Address::setPort( u16_t port ) {
    m_addr.sin_port = htons(port);
}

void Inet4Address::setAddress( u32_t address ) {
    m_addr.sin_addr.s_addr = address;
}

std::string Inet4Address::toString() const {
    char buf[128];

    u32_t a = ntohl(m_addr.sin_addr.s_addr);
    snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%d",
        a >> 24, (a >> 16) & 0xFF,
        (a >> 8) & 0xFF, a & 0xFF, ntohs(m_addr.sin_port) );
    return buf;
}

using namespace std;
SocketAddress* SocketAddress::parse(const char* str_) {
    string str = str_;
    size_t idx;
    if(str.compare(0, 6, "unix:/") == 0) {
        if(str.size() < 7 || str[6] != '/') {
            throw SocketAddressParseException(strprintf("Unable to parse UnixAddress (%s)", 4096, str_).c_str());
        }
        return new io::UnixAddress(str.substr(6).c_str());
    } else if((idx = str.find(':')) != std::string::npos &&
        (str.rfind(':') != idx)) { /* If there are 2 ':' in the address */
        io::Inet6Address* ret = new io::Inet6Address(
            io::Inet6Address::fromString(str_)
        );
        return ret;
    } else {
        io::Inet4Address* ret = new io::Inet4Address(
            io::Inet4Address::fromString(str_)
        );
        return ret;
    }
}

}
