#include <io/Inet6Address.hpp>

#include <arpa/inet.h>
#include <algorithm>
#include <cstdio>

using namespace std;

namespace io {

Inet6Address Inet6Address::fromString( const char* ipv6, u16_t port ) {
    u8_t nil[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    Inet6Address ret(nil, port);

    if (inet_pton(AF_INET6, ipv6, &ret.m_addr.sin6_addr) == 1)
        return ret;

    char buf[1024];
    snprintf(buf, sizeof(buf), "Failed to pares ipv6 address: %s", ipv6);
    throw Inet6ParseException(buf);  
}

Inet6Address::Inet6Address(u8_t addr[16], u16_t port) {
    fill( (byte*)&m_addr, (byte*)&m_addr + sizeof(m_addr), 0 );
    m_addr.sin6_family = AF_INET6;
    m_addr.sin6_port = htons(port);
    copy( addr, addr + 16, m_addr.sin6_addr.s6_addr );
}

#if defined(__OpenBSD__)
#define s6_addr16 ((u16_t*)s6_addr)
#endif
std::string Inet6Address::toString() const {
    char buf[64];
	u16_t* m_addr16 = (u16_t*) m_addr.sin6_addr.s6_addr;
    snprintf(buf, sizeof(buf), "[%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x]:%d",
        m_addr16[0],
        m_addr16[1],
        m_addr16[2],
        m_addr16[3],
        m_addr16[4],
        m_addr16[5],
        m_addr16[6],
        m_addr16[7],
        ntohs(m_addr.sin6_port));
    return buf;
}

void Inet6Address::setPort(u16_t port) {
    m_addr.sin6_port = htons(port);
}

u16_t Inet6Address::getPort() {
    return ntohs(m_addr.sin6_port);
}

}
