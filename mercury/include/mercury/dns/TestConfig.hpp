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

#include <json/JsonMercuryTempl.hpp>
#include <json/Json.hpp>

namespace dns {

class TestConfig {
public:
    TestConfig(){}
    std::vector<std::string> valid_hostnames;
    std::vector<std::string> invalid_hostnames;
    uptr<io::SocketAddress> server_address;
    os::timeout_t timeout_micros;
};

}

template <>
struct JsonBasicConvert<dns::TestConfig> {
    static dns::TestConfig convert(const json::Json& jsn) {
        dns::TestConfig ret;
        jsn["valid_names"].toVector(ret.valid_hostnames);
        jsn["invalid_names"].toVector(ret.invalid_hostnames);
        ret.server_address = jsn["dns_server"].convert<io::SocketAddress*>();
        ret.timeout_micros = jsn["timeout"].intValue() * 1000;
        return ret;
    }
};

#endif /* MERCURY_TESTS_dns_TESTCONFIG_HPP_ */
