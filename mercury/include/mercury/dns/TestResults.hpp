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
    f64_t avg_response_time_secs;
    f64_t response_time_stdev;
    f64_t packets_lost;
};

}

#endif /* MERCURY_TESTS_dns_TESTRESULTS_HPP_ */
