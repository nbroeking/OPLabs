#ifndef MERCURY_TESTS_throughput_TESTCONFIG_HPP_
#define MERCURY_TESTS_throughput_TESTCONFIG_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * TestConfig.hpp: <description>
 */

#include <vector>
#include <io/SocketAddress.hpp>

#include <json/Json.hpp>

namespace throughput {

class TestConfig {
public:
    uptr<io::SocketAddress> server_ip;
};

}

template <>
struct JsonBasicConvert<throughput::TestConfig> {
    static throughput::TestConfig convert(const json::Json& jsn) {
        throughput::TestConfig ret;
        ret.server_ip = jsn["server_ip"].convert<io::SocketAddress*>();
        return ret;
    }
};

#endif /* MERCURY_TESTS_throughput_TESTCONFIG_HPP_ */
