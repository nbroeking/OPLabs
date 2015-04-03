#ifndef MERCURY_TESTS_dns_TESTCONFIG_HPP_
#define MERCURY_TESTS_dns_TESTCONFIG_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * TestConfig.hpp: <description>
 */

#include <vector>
#include <io/Inet4Address.hpp>
#include <os/Time.hpp>

#include <jansson.h>

namespace dns {

class TestConfig {
public:
    std::vector<std::string> valid_hostnames;
    std::vector<std::string> invalid_hostnames;
    std::vector< uptr<io::SocketAddress> > dns_servers;
    os::timeout_t timeout_micros;

    /**
     * Attempt to parse a TestConfig for dns
     */
    static TestConfig parse(json_t* json);
};

}

#endif /* MERCURY_TESTS_dns_TESTCONFIG_HPP_ */
