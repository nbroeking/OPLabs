#ifndef UNIXADDRESS_HPP_
#define UNIXADDRESS_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * UnixAddress.hpp: <description>
 */

#include <io/SocketAddressTempl.hpp>
#include <prelude.hpp>

#include <sys/un.h>

namespace io {

class UnixAddress : public SocketAddressTempl<sockaddr_un, AF_UNIX> {
public:
    UnixAddress(const char* address);
    inline UnixAddress( const sockaddr_un& raw ) { m_addr = raw; };
    virtual socklen_t rawlen() const OVERRIDE ;

    void unlink();
};

}

#endif /* UNIXADDRESS_HPP_ */
