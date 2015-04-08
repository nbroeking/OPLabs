#ifndef MERCURY_TESTS_throughput_PINGTESTPROXY_HPP_
#define MERCURY_TESTS_throughput_PINGTESTPROXY_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * PingTestProxy.hpp: <description>
 */

#include <mercury/throughput/TestObserver.hpp>
#include <mercury/throughput/TestConfig.hpp>

namespace throughput {

/**
 * A proxy to a ping test. Designed to maybe be implemented in a
 * separate process from the calling client code.
 */
class TestProxy {
public:
    /**
     * Begin the test
     */
    virtual void start(const TestConfig& conf, TestObserver* observer) = 0;
};

}

#endif /* MERCURY_TESTS_throughput_PINGTESTPROXY_HPP_ */
