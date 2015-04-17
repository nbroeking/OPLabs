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
    std::vector<u64_t> download_throughput_per_sec;

    /* List of the latency of the UDP latency
     * test during the load time */
    std::vector<os::timeout_t> download_latency_micros;

    std::vector<u64_t> upload_throughput_per_sec;
    std::vector<os::timeout_t> upload_latency_micros;
};

}

template<>
struct JsonBasicConvert<throughput::TestResults> {
    static inline json::Json convert_from(const throughput::TestResults& results) {
        using namespace json;
        using namespace std;

        Json down_throughputs = Json::from(results.download_throughput_per_sec);

        vector<Json> vec;
        Json down_latencies = Json::fromVector(vec);

        vector<os::timeout_t>::const_iterator itr;
        FOR_EACH(itr, results.download_latency_micros) {
            down_latencies.push_back(Json::fromFloat(*itr / 1000.0));
        }

        Json up_throughputs = Json::from(results.upload_throughput_per_sec);
        Json up_latencies = Json::fromVector(vec);

        FOR_EACH(itr, results.upload_latency_micros) {
            up_latencies.push_back(Json::fromFloat(*itr / 1000.0));
        }

        Json ret;
        ret.setAttribute("download_throughputs", down_throughputs);
        ret.setAttribute("download_latencies", down_latencies);
        ret.setAttribute("upload_throughputs", up_throughputs);
        ret.setAttribute("upload_latencies", up_latencies);
        return ret;
    }
};

#endif /* MERCURY_TESTS_throughput_TESTRESULTS_HPP_ */
