#ifndef MERCURY_TESTS_dns_TEST_HPP_
#define MERCURY_TESTS_dns_TEST_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * Test.hpp: <description>
 */

#include <Prelude>
#include <mercury/dns/TestProxy.hpp>

namespace dns {

class Test {
public:
    /**
     * Return the instance of this test proxy
     */
    static TestProxy& instance();

private:
    /* The singleton */
    static TestProxy* m_instance;
};

}

#endif /* MERCURY_TESTS_dns_TEST_HPP_ */
