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
#include <sys/socket.h>

namespace io {

/**
 * @brief Socket address that defines a UNIX socket
 */
class UnixAddress : public SocketAddressTempl<sockaddr_un, AF_UNIX> {
public:
    /**
     * @brief Create a UnixAddress from the char* given
     * @param address The address of the named socket on the filesystem
     */
    UnixAddress(const char* address);

    /**
     * @brief Build a UnixAddress from the plain old C version
     * @param raw The plain old C version
     */
    inline UnixAddress( const sockaddr_un& raw ) { m_addr = raw; };

    /**
     * @brief The raw length of the socketaddress
     * @return the raw length of the socketaddress
     */
    virtual socklen_t rawlen() const OVERRIDE ;

    /**
     * @brief Print the string representation of this SocketAddress
     * @return the string representation of this SocketAddress
     */
    std::string toString() const ;

    /**
     * @brief Deletes the named socket from the filesystem.
     */
    void unlink();

    virtual ~UnixAddress();
};

}

#endif /* UNIXADDRESS_HPP_ */
