#include <io/SocketAddress.hpp>

#include <io/Inet4Address.hpp>
#include <io/Inet6Address.hpp>
#include <io/UnixAddress.hpp>

#include <cstdio>

namespace io {

SocketAddress* SocketAddress::toSocketAddress( struct sockaddr* addr, socklen_t len ) {
    (void) len ;

    switch( addr->sa_family ) {
    case AF_INET:
        return new Inet4Address( *(struct sockaddr_in*)addr );
    case AF_INET6:
        return new Inet6Address( *(struct sockaddr_in6*)addr );
    case AF_UNIX:
        return new UnixAddress( *(struct sockaddr_un*)addr );
    }

    char buf[1024];
    snprintf( buf, sizeof(buf), "Unknown socket family %d", addr->sa_family );
    throw UnknownSocketFamilyException(buf);
}

}
