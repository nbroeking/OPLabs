#include <mercury/ping/Test.hpp>
#include <io/ICMPSocket.hpp>
#include <proc/StateMachine.hpp>
#include <proc/Process.hpp>
#include <unistd.h>

using namespace io;
using namespace os;
using namespace std;
using namespace proc;

namespace ping {

enum Stim {
      BEGIN_TEST
    , CANCEL_TEST
    , MAP_EMPTY
    , TIMEOUT
};

enum State {
      IDLE
    , PINGS_SENT
};

ENUM_TO_STRING(Stim, 4, "BeginTest", "CancelTest", "MapEmpty", "Timeout")
ENUM_TO_STRING(State, 2, "Idle", "PingsSent", "Timeout")

class PingTestImpl: public ProtectedProcess, FileCollectionObserver {
public:
    PingTestImpl(): ProtectedProcess("PingTest"),
                    m_state_machine(*this, IDLE, getScheduler()) {
        /* Build up the state machine */
        setup_state_machine();
        if(m_socket.init()) {
            printf("this %p\n", this);
            m_log->printfln(ERROR, "Error initializing icmp socket. (need root?)");
        }

        /* Attach this to be the consumer of data from the ICMP socket */
        getFileCollection().subscribe(FileCollection::SUBSCRIBE_READ, &m_socket, this);
    }

    /* Begin implementation of non-state machine related {{{ */
    virtual void observe(int fd, int events) OVERRIDE {
        (void) fd; (void) events;
        m_log->printfln(TRACE, "Observation on ICMP socket");
        ICMPPacket packet;
        uptr<SocketAddress> addr;
        m_socket.receive(packet, addr.get());

        m_log->printfln(TRACE, "Ping received from %s", addr->toString().c_str());

        ICMPHeader header = packet.getHeader();
        u16_t echo_id = header.getEchoId();
        u16_t echo_seq = header.getEchoSequence();

        {   ScopedLock __sl(m_ds_mutex);
            map<u32_t,ping_data>::iterator itr;
            itr = m_table.find((echo_id << 16) | echo_seq);

            if(itr == m_table.end()) {
                m_log->printfln(WARN, "Unsolicited echo from %s", addr->toString().c_str());
            } else {
                ping_data data = (*itr).second;
                timeout_t current = Time::currentTimeMicros();
                timeout_t diff = current - data.time;
                m_table.erase(itr);

                m_log->printfln(TRACE, "Ping received from %s (id=%hu,seq=%hu; target=%s, time=%dus, this=%p)",
                    addr->toString().c_str(), echo_id, echo_seq, data.addr->toString().c_str(),
                    diff, this);

                if(m_table.empty()) {
                    m_state_machine.sendStimulus(MAP_EMPTY);
                }

                data.time = diff;
                results.push_back(data);
            }
        }

    }

    void setup_state_machine() {
        m_state_machine.setEdge(IDLE, BEGIN_TEST, &PingTestImpl::on_start_test);
        m_state_machine.setEdge(PINGS_SENT, MAP_EMPTY, &PingTestImpl::on_finish);
        m_state_machine.setEdge(PINGS_SENT, TIMEOUT, &PingTestImpl::on_finish);
    }
    /* }}} */

    /* Begin implemenation of state machine methods {{{ */
    State on_start_test() {
        /* The test should be starting */
        /* send off a bunch of pings */

        u16_t echo_id = rand();
        u16_t echo_seq = 0;

        vector< uptr<SocketAddress> >::iterator itr;
        for( ; echo_seq < 10 ; ++ echo_seq ) {
            FOR_EACH(itr, m_config.ping_addrs) {
                m_log->printfln(DEBUG, "Sending echo request (id=%hu,seq=%hu)", echo_id, echo_seq);
    
    
                {   ScopedLock __sl(m_ds_mutex);
                    ping_data& data = m_table[(echo_id << 16) | echo_seq];
                    data.time = Time::currentTimeMicros();
                    data.addr = *itr;
                    m_log->printfln(TRACE, "Write data target=%s time=%d to data slot %d (this=%p)",
                        data.addr->toString().c_str(), data.time, echo_id, this);
                    send_ping(*itr->get(), echo_id, echo_seq);
                }
                ++ echo_id;

            }
        }

        /* In theory, all the pings should return eventually,
         * but in case they did not, set a stim to timeout after a
         * second */
        m_log->printfln(TRACE, "Set timeout stim for %ld\n", 5 SECS);
        m_state_machine.setTimeoutStim(5 SECS, TIMEOUT);
        return PINGS_SENT;
    }

    State on_finish() {
        /* This is called if all pings were sucessfully returned
         * or if the 1 second timeout was reached */
        u64_t total_time = 0;
        size_t cnt = 0;

        vector<ping_data>::iterator itr;
        FOR_EACH(itr, results) {
            total_time += itr->time;
            cnt ++;
        }

        f64_t average = total_time / ((f64_t)cnt);

        map<u32_t, ping_data>::iterator mitr;
        cnt = 0;
        FOR_EACH(mitr, m_table) {
            /* count all the unreturned pings */
            cnt ++;
        }
        m_table.clear();

        m_log->printfln(INFO, "Ping time average %fus", average);
        m_log->printfln(INFO, "%d lost packets", cnt);

        TestResults results;
        results.avg_latency_micros = average;
        results.packets_lost = cnt;

        m_observer->onTestComplete(results);

        return IDLE;
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

    struct ping_data {
        timeout_t time;
        uptr<SocketAddress> addr;
    };

    void send_ping(SocketAddress& to, u16_t echo_id, u16_t echo_seq) {
        ICMPHeader header = ICMPHeader::Echo(echo_id, echo_seq);
        ICMPPacket packet;

        packet.setHeader(header);
        packet.setMessage((const byte*)"TestPing", sizeof("TestPing")-1);

        m_socket.send(packet, to);
    }

    vector<ping_data> results;

    StateMachine<Stim, PingTestImpl, State> m_state_machine;
    ICMPSocket m_socket;

    Condition m_condition;

    Mutex m_ds_mutex;
    map<u32_t, ping_data> m_table;


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
