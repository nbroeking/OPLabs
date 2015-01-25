#ifndef UNSPECIFIEDADDRESS_HPP_
#define UNSPECIFIEDADDRESS_HPP_

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

/**
 * @brief SocketAddress for IPv4
 */
class UnspecifiedAddress : public SocketAddressTempl<sockaddr, AF_UNSPEC> {
public:
    /**
     * @brief Create an Inet4Address from a 4-byte address and port
     * 
     * @param addr The ip address
     * @param port The port number
     */
    inline UnspecifiedAddress() {
		m_addr.sa_family = AF_UNSPEC;
	}

    virtual inline std::string toString() const OVERRIDE { return "(Unspecified)"; }
};

}

#endif /* INET4ADDRESS_HPP_ */
