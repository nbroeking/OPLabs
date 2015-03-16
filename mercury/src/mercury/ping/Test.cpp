#include <mercury/ping/Test.hpp>
#include <io/ICMPSocket.hpp>
#include <proc/StateMachine.hpp>
#include <proc/Process.hpp>

using namespace io;
using namespace os;
using namespace std;
using namespace proc;

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

ENUM_TO_STRING(Stim, 3, "BeginTest", "CancelTest", "PingReceived")
ENUM_TO_STRING(State, 2, "Idle", "PingsSent")

class PingTestImpl: public ProtectedProcess, FileCollectionObserver {
public:
    PingTestImpl(): ProtectedProcess("PingTest"),
                    m_state_machine(*this, IDLE) {
        /* Build up the state machine */
        setup_state_machine();
        if(m_socket.init()) {
            m_log.printfln(ERROR, "Error initializing icmp socket. (need root?)");
        }

        /* Attach this to be the consumer of data from the ICMP socket */
        getFileCollection().subscribe(FileCollection::SUBSCRIBE_READ, &m_socket, this);
    }

    /* Begin implementation of non-state machine related {{{ */
    virtual void observe(int fd, int events) OVERRIDE {
        (void) fd; (void) events;
        m_log.printfln(DEBUG, "Observation on ICMP socket");
        ICMPPacket packet;
        uptr<SocketAddress> addr;
        m_socket.receive(packet, addr.get());

        m_log.printfln(DEBUG, "Ping received from %s", addr->toString().c_str());
    }

    void setup_state_machine() {
        m_state_machine.setEdge(IDLE, BEGIN_TEST, &PingTestImpl::on_start_test);
    }
    /* }}} */

    /* Begin implemenation of state machine methods {{{ */
    State on_start_test() {
        /* The test should be starting */
        /* send off a bunch of pings */

        u16_t echo_id = rand();
        u16_t echo_seq = 0;

        vector<Inet4Address>::iterator itr;
        FOR_EACH(itr, m_config.ping_addrs) {
            send_ping(*itr, echo_id, echo_seq);
        }

        /* In theory, all the pings should return eventually */
        return PINGS_SENT;
    }
    /* }}} */
    
    void set_configuration(const TestConfig& conf) {
        m_config = conf;
    }

    void set_observer(TestObserver* observer) {
        m_observer = observer;
    }

    State getCurrentState() {
        return m_state_machine.getCurrentState();
    }

    /* implement Process {{{ */
    void start_test() {
        ScopedLock __sl(m_mutex);
        m_condition.signal();
    }

    void run() {
        /* m_mutex is already locked */
        while (true) {
            m_condition.wait(m_mutex);
            m_state_machine.sendStimulus(BEGIN_TEST);
        }
    }
    /* }}} */
private:

    void send_ping(SocketAddress& to, u16_t echo_id, u16_t echo_seq) {
        ICMPHeader header = ICMPHeader::Echo(echo_id, echo_seq);
        ICMPPacket packet;

        packet.setHeader(header);
        packet.setMessage((const byte*)"TestPing", sizeof("TestPing"));

        m_socket.send(packet, to);
    }

    StateMachine<Stim, PingTestImpl, State> m_state_machine;
    ICMPSocket m_socket;

    Condition m_condition;

    /* this is the configuration that needs to be set {{{ */
    TestConfig m_config;
    TestObserver* m_observer;
    /* }}} */
};

class TestProxyImpl: public TestProxy {
public:
    TestProxyImpl() {
        /* start the tester thread */
        tester.start();
    }
    virtual void start(const TestConfig& conf, TestObserver* observer) {
        ScopedLock __sl(m_mutex);
        if(tester.getCurrentState() == IDLE) {
            tester.set_configuration(conf);
            tester.set_observer(observer);
            tester.start_test();
        }
    };

private:
    Mutex m_mutex;
    PingTestImpl tester;
};


TestProxy* Test::m_instance;
TestProxy& Test::instance() {
    if(!m_instance) {
        m_instance = new TestProxyImpl();
    }
    return *m_instance;
}

}
