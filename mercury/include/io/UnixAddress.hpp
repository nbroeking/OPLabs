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

    void unlink();
};

}

#endif /* UNIXADDRESS_HPP_ */
