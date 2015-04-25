#ifndef MERCURY_TESTS_$$_TEST_HPP_
#define MERCURY_TESTS_$$_TEST_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * Test.hpp: <description>
 */

#include <Prelude>
#include <mercury/$$/TestProxy.hpp>

namespace $$ {

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

#endif /* MERCURY_TESTS_$$_TEST_HPP_ */
