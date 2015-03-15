#ifndef TEST_HPP_
#define TEST_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * Test.hpp: <description>
 */

#include <Prelude>
#include <mercury/ping/TestProxy.hpp>

namespace ping {

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

#endif /* TEST_HPP_ */
