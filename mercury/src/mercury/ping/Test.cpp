#include <mercury/ping/Test.hpp>
#include <io/ICMPSocket.hpp>
#include <proc/StateMachine.hpp>

using namespace io;

namespace ping {

enum Stim {
      BEGIN_TEST
    , CANCEL_TEST
    , PING_RECIEVED
};

enum State {
      IDLE
    , PINGS_SENT
};

class PingTestImpl {
public:
    void setup_state_machine() {
        m_state_machine.setEdge(IDLE, BEGIN_TEST, &PingTestImpl::on_start_test);
    }

    State on_start_test() {
        /* The test should be starting */
        /* send off a bunch of pings */

        return PINGS_SENT;
    }
    
private:
    StateMachine<Stim, PingTestImpl, State> m_state_machine;
    ICMPSocket m_socket;
};


TestProxy* Test::m_instance;
TestProxy& Test::instance() {
    return *m_instance;
}

}
