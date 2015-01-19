#include <io/Inet4Address.hpp>
#include <netinet/in.h>

namespace io {

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
        throw "Error";
    }
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
}

void Inet4Address::setPort( u16_t port ) {
    m_addr.sin_port = htons(port);
}

void Inet4Address::setAddress( u32_t address ) {
    m_addr.sin_addr.s_addr = address;
}

}
