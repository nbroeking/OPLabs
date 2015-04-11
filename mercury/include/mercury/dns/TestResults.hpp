#ifndef MERCURY_TESTS_dns_TESTRESULTS_HPP_
#define MERCURY_TESTS_dns_TESTRESULTS_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * TestResults.hpp: <description>
 */

#include <Prelude>

#include <json/Json.hpp>
#include <cmath>

namespace dns {

class TestResults {
public:
    /* times in MICRO seconds */
    f64_t valid_avg_response_time_mircos;
    f64_t valid_response_time_stdev;

    f64_t invalid_avg_response_time_mircos;
    f64_t invalid_response_time_stdev;

    /* packets_lost / total_packets */
    f64_t packets_lost_ratio;

};


inline json::Json cov_float(float f) {
    using namespace json;
    if(std::isnan(f)) {
        return Json::fromFloat(-1.0);
    } else {
        return Json::fromFloat(f);
    }
}

}

template <>
struct JsonBasicConvert<dns::TestResults> {
    static json::Json convert_from(const dns::TestResults& str) {
        using namespace json;
        Json ret;

        Json tmp;

        tmp = dns::cov_float(str.valid_avg_response_time_mircos / 1000.0);
        ret.setAttribute("latency_avg", tmp);

        tmp = dns::cov_float(str.valid_response_time_stdev / 1000.0);
        ret.setAttribute("latency_sdev", tmp);

        tmp = dns::cov_float(str.invalid_avg_response_time_mircos / 1000.0);
        ret.setAttribute("dns_response_avg", tmp);

        tmp = dns::cov_float(str.invalid_response_time_stdev / 1000.0);
        ret.setAttribute("dns_response_sdev", tmp);

        tmp = dns::cov_float(str.packets_lost_ratio / 1000.0);
        ret.setAttribute("packet_loss", tmp);

        return ret;
    }
};

#endif /* MERCURY_TESTS_dns_TESTRESULTS_HPP_ */
