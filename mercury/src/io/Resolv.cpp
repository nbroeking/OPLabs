#include <io/Resolv.hpp>
#include <netdb.h>

namespace io {

using namespace std;

Inet4Address Resolv::getHostByName4( const char* name ) {
    struct addrinfo* info;
    struct addrinfo* cur ;

    int err;
    err = getaddrinfo( name, NULL, NULL, &info );

    if ( err ) {
        throw ResolvException("Error in getaddrinfo", err);
    }

    
    for( cur = info; cur != NULL; cur = cur->ai_next ) {
        if ( cur->ai_family == AF_INET ) {
            Inet4Address ret = Inet4Address(*(sockaddr_in*)cur->ai_addr);
            freeaddrinfo(info);
            return ret;
        }
    }

    throw ResolvException("No such IPv4 host", -255);
}

vector< uptr<SocketAddress> > Resolv::getHostByName(const char* name) {
    struct addrinfo* info;
    struct addrinfo* cur ;

    int err;
    err = getaddrinfo( name, NULL, NULL, &info );
    vector< uptr<SocketAddress> > ret;

    if ( err ) {
        throw ResolvException("Error in getaddrinfo", err);
    }

    
    for( cur = info; cur != NULL; cur = cur->ai_next ) {
        SocketAddress* next = SocketAddress::toSocketAddress(cur->ai_addr, cur->ai_addrlen);
        ret.push_back(next);
    }

    freeaddrinfo(info);
    return ret;
}

}
