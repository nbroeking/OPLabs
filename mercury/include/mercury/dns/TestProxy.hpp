#ifndef MERCURY_TESTS_dns_PINGTESTPROXY_HPP_
#define MERCURY_TESTS_dns_PINGTESTPROXY_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * PingTestProxy.hpp: <description>
 */

#include <mercury/dns/TestObserver.hpp>
#include <mercury/dns/TestConfig.hpp>

namespace dns {

/**
 * A proxy to a ping test. Designed to maybe be implemented in a
 * separate process from the calling client code.
 */
class TestProxy {
public:
    /**
     * Begin the test
     */
    virtual void startTest(const TestConfig& conf, TestObserver* observer) = 0;

    virtual void stop() = 0;
};

}

#endif /* MERCURY_TESTS_dns_PINGTESTPROXY_HPP_ */
