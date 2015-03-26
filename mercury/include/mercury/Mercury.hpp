#ifndef MERCURY_HPP_
#define MERCURY_HPP_

/*
 * Author: jrahm
 * created: 2015/03/26
 * Mercury.hpp: <description>
 */

#include <mercury/Config.hpp>
#include <mercury/Callback.hpp>

namespace mercury {

/**
 * Proxy class for an implementation of mercury
 */
class Proxy {
public:
    /* Interface */

    /**
     * Start mercury
     */
    virtual void start(const Config& conf, Callback* callback) = 0;
};

/**
 * Holder for a singleton proxy
 */
class ProxyHolder {
public:
    static Proxy& instance();
};

}

#endif /* MERCURY_HPP_ */
