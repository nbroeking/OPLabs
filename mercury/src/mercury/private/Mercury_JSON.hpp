#include <Prelude>
#include <io/SocketAddress.hpp>
#include <io/Inet4Address.hpp>

#include <mercury/dns/TestConfig.hpp>
#include <mercury/throughput/TestConfig.hpp>

#include <json/Json.hpp>

#include <vector>

using namespace std;
using namespace io;

class MercuryTestConfig {
public:
    dns::TestConfig dns_test_config;
    throughput::TestConfig throughput_test_config;
};

template <>
struct JsonBasicConvert<MercuryTestConfig> {
    static MercuryTestConfig convert(const json::Json& jsn) {
        MercuryTestConfig ret;
        ret.dns_test_config = jsn["dns_config"].convert<dns::TestConfig>();
        ret.throughput_test_config = jsn["throughput_config"].convert<throughput::TestConfig>();
        return ret;
    }
};

inline MercuryTestConfig parseMercuryTestConfig(const char* jsn_str, bool& fail) {
    uptr<json::Json> jsn = json::Json::parse(jsn_str);

    if((*jsn)["status"] == "failure") {
        fail = true;
    } else {
        fail = false;
    }

    return (*jsn)["config"].convert<MercuryTestConfig>();
}
