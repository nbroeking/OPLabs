#ifndef MERCURY_TESTS_dns_TESTRESULTS_HPP_
#define MERCURY_TESTS_dns_TESTRESULTS_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * TestResults.hpp: <description>
 */

#include <Prelude>

namespace dns {

class TestResults {
public:
    /* times in MICRO seconds */
    f64_t valid_avg_response_time_mircos;
    f64_t valid_response_time_stdev;
    s64_t valid_packets_lost;

    f64_t invalid_avg_response_time_mircos;
    f64_t invalid_response_time_stdev;
    s64_t invalid_packets_lost;
};

}

#endif /* MERCURY_TESTS_dns_TESTRESULTS_HPP_ */
