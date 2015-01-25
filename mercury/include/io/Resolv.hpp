#ifndef RESOLV_HPP_
#define RESOLV_HPP_

/*
 * Author: jrahm
 * created: 2015/01/17
 * Resolv.hpp: <description>
 */

#include <types.h>

#include <io/SocketAddress.hpp>
#include <io/Inet4Address.hpp>

#include <string>
#include <vector>

#include <prelude.hpp>

namespace io {

class ResolvException {
public:
    inline ResolvException(const std::string& err, int rc): err(err), rc(rc) {}
    inline const std::string& getError() const { return err; }
    inline int getCode() { return rc; }
private:
    std::string err;
    int rc;
};

class Resolv {
public:
    static Inet4Address getHostByName4( const char* name );
    static std::vector< uptr<SocketAddress> > getHostByName(const char* name);
};

}

#endif /* RESOLV_HPP_ */
