#ifndef SOCKETADDRESSTEMPL_HPP_
#define SOCKETADDRESSTEMPL_HPP_

/*
 * Author: jrahm
 * created: 2015/01/17
 * SocketAddress.hpp: <description>
 */

#include <io/SocketAddress.hpp>

namespace io {

template <typename _Addr_Str, int _Proto>
class SocketAddressTempl : public SocketAddress {
public:
    /**
     * The raw c-style sockaddr that the base class represents
     */
    virtual inline struct sockaddr* raw() const {return (sockaddr*) &m_addr;};

    /**
     * @brief returns the length of the sockaddr returned by `raw()`
     * @return the length of the sockaddr returned by `raw()`
     */
    virtual inline socklen_t rawlen() const {return sizeof(_Addr_Str);};

    /**
     * @brief returns the correct layer 3 protocol for this address
     * @return the correct layer 3 protocol for this address
     */
    virtual inline int linkProtocol() const {return _Proto;};

    virtual inline ~SocketAddressTempl() {}
protected:
    _Addr_Str m_addr;
};

}

#endif /* SOCKETADDRESS_HPP_ */
