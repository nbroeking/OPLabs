#ifndef PINGTESTPROXY_HPP_
#define PINGTESTPROXY_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * PingTestProxy.hpp: <description>
 */

#include <mercury/ping/TestObserver.hpp>

namespace ping {

/**
 * A proxy to a ping test. Designed to maybe be implemented in a
 * separate process from the calling client code.
 */
class TestProxy {
public:
    /**
     * Begin the test
     */
    virtual void start(TestObserver* observer) = 0;
};

}

#endif /* PINGTESTPROXY_HPP_ */
