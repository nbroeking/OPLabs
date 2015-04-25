#ifndef TESTRESULTS_HPP_
#define TESTRESULTS_HPP_

/*
 * Author: jrahm
 * created: 2015/03/15
 * TestResults.hpp: <description>
 */

#include <Prelude>

namespace ping {

class TestResults {
public:
    double avg_latency_micros;
    u32_t packets_lost;
};

}

#endif /* TESTRESULTS_HPP_ */
