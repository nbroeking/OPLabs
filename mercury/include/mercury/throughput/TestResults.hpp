#ifndef MERCURY_TESTS_throughput_TESTRESULTS_HPP_
#define MERCURY_TESTS_throughput_TESTRESULTS_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * TestResults.hpp: <description>
 */

#include <Prelude>

#include <vector>

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

#endif /* MERCURY_TESTS_throughput_TESTRESULTS_HPP_ */
