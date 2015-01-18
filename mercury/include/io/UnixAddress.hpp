#ifndef UNIXADDRESS_HPP_
#define UNIXADDRESS_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * UnixAddress.hpp: <description>
 */

#include <io/SocketAddress.hpp>
#include <prelude.hpp>

#include <sys/un.h>

namespace io {

class UnixAddress : public SocketAddress {
public:
    UnixAddress(const char* address);

    virtual sockaddr* raw() const OVERRIDE;
    virtual socklen_t rawlen() const OVERRIDE ;
    virtual inline int linkProtocol() const OVERRIDE { return AF_UNIX; }

    void unlink();
private:
    struct sockaddr_un m_addr;
};

}

#endif /* UNIXADDRESS_HPP_ */
