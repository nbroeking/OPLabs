#ifndef MERCURY_TESTS_throughput_TEST_HPP_
#define MERCURY_TESTS_throughput_TEST_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * Test.hpp: <description>
 */

#include <Prelude>
#include <mercury/throughput/TestProxy.hpp>

namespace throughput {

class Test {
public:
    /**
     * Return the instance of this test proxy
     */
    static TestProxy* createInstance();
};

}

#endif /* MERCURY_TESTS_throughput_TEST_HPP_ */
