#ifndef MERCURY_TESTS_throughput_TESTRESULTS_HPP_
#define MERCURY_TESTS_throughput_TESTRESULTS_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * TestResults.hpp: <description>
 */

#include <Prelude>

#include <vector>
#include <json/Json.hpp>
#include <json/JsonMercuryTempl.hpp>

#include <os/Time.hpp>

namespace throughput {

class TestResults {
public:
    /* list of number of bytes recieved each
     * second */
    std::vector<u64_t> throughput_per_sec;

    /* List of the latency of the UDP latency
     * test during the load time */
    std::vector<os::timeout_t> latency_micros;
};

}

template<>
struct JsonBasicConvert<throughput::TestResults> {
    static inline json::Json convert_from(const throughput::TestResults& results) {
        using namespace json;
        using namespace std;

        Json throughputs = Json::from(results.throughput_per_sec);

        vector<Json> vec;
        Json latencies = Json::fromVector(vec);

        vector<os::timeout_t>::const_iterator itr;
        FOR_EACH(itr, results.latency_micros) {
            latencies.push_back(Json::fromFloat(*itr / 1000.0));
        }

        Json ret;
        ret.setAttribute("throughputs", throughputs);
        ret.setAttribute("latencies", latencies);
        return ret;
    }
};

#endif /* MERCURY_TESTS_throughput_TESTRESULTS_HPP_ */
