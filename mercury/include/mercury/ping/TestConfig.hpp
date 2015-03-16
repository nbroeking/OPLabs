#ifndef TESTCONFIG_HPP_
#define TESTCONFIG_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * TestConfig.hpp: <description>
 */

#include <vector>
#include <io/Inet4Address.hpp>

namespace ping {

class TestConfig {
public:
    /* the list of address to test ping speed to */
    std::vector<io::Inet4Address> ping_addrs;
};

}

#endif /* TESTCONFIG_HPP_ */
