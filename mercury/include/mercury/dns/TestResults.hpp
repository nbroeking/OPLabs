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


inline bool cov_float(float f, json::Json& out) {
    using namespace json;
    if(std::isnan(f)) {
        return false;
    } else {
        out = Json::fromFloat(f);
        return true;
    }
}

}

template <>
struct JsonBasicConvert<dns::TestResults> {
    static json::Json convert_from(const dns::TestResults& str) {
        using namespace json;
        Json ret;

        Json tmp;

        if(dns::cov_float(str.valid_avg_response_time_mircos / 1000.0, tmp))
            ret.setAttribute("latency_avg", tmp);

        if(dns::cov_float(str.valid_response_time_stdev / 1000.0, tmp))
            ret.setAttribute("latency_sdev", tmp);

        if(dns::cov_float(str.invalid_avg_response_time_mircos / 1000.0, tmp))
            ret.setAttribute("dns_response_avg", tmp);

        if(dns::cov_float(str.invalid_response_time_stdev / 1000.0, tmp))
            ret.setAttribute("dns_response_sdev", tmp);

        if(dns::cov_float(str.packets_lost_ratio, tmp))
            ret.setAttribute("packet_loss", tmp);

        return ret;
    }
};

#endif /* MERCURY_TESTS_dns_TESTRESULTS_HPP_ */
