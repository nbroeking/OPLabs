#include <mercury/dns/Test.hpp>
#include <io/DatagramSocket.hpp>

#include <proc/StateMachine.hpp>
#include <proc/Process.hpp>

#include <cmath>

using namespace io;
using namespace os;
using namespace proc;
using namespace std;

namespace dns {

enum Stim {
      BEGIN_TEST /* starts a test */
    , PACKET_RECEIEVED
    , STOP_TEST
    , TIMEOUT
};

enum State {
      IDLE
    , PACKET_SENT
};

ENUM_TO_STRING(Stim, 4, "BeginTest", "PacketReceived", "StopTest", "Timeout");
ENUM_TO_STRING(State, 2, "Idle", "PacketSent");

class DnsTestImpl: public Process,
                   public FileCollectionObserver,
                   public TestProxy
                   {
public:
    DnsTestImpl(): Process("DnsTest") {
        m_state_machine = new StateMachine<Stim, DnsTestImpl, State>(*this, IDLE, this->getScheduler());
        m_state_machine->setEdge(IDLE, BEGIN_TEST, &DnsTestImpl::onBeginTest);
        m_state_machine->setEdge(PACKET_SENT, PACKET_RECEIEVED, &DnsTestImpl::onPacketReceieved);
        m_state_machine->setEdge(PACKET_SENT, TIMEOUT, &DnsTestImpl::onTimeout);
    }
    /* wait for observations in the DnsTestImpl */
    void observe(int fd, int events) OVERRIDE {
        (void) fd; (void) events;
        m_log->printfln(TRACE, "Observation in udp socket");

        byte bytes[1024];
        SocketAddress* addr;
        ssize_t bytes_read;

        m_socket.receive(bytes, sizeof(bytes), addr);
        m_state_machine->sendStimulus(PACKET_RECEIEVED);
    }

    State onBeginTest() {
        m_socket.bind(Inet4Address("0.0.0.0", 0));
        enqueue_request_vector(conf.valid_hostnames);
        packets_sent = 0;
        m_testing_invalid = false;

        m_latency_times = &m_valid_latency_times;
        m_valid_latency_times.clear();
        m_invalid_latency_times.clear();

        if(!m_waiting_to_resolve.empty()) {
            to_resolve = m_waiting_to_resolve.front();
            m_waiting_to_resolve.pop();
            return send_dns();
        } else {
            return IDLE;
        }
    }

    State onTimeout() {
        m_latency_times->push_back(-1);
        return send_dns();
    }

    State onPacketReceieved() {
        /* Add to the latency times */
        timeout_t to = Time::currentTimeMicros();
        m_latency_times->push_back(to - sent_time);
        if(packets_sent < 50) {
            /* we send 50 dns requests for each
             * host to resolve */
            return send_dns();
        } else {
            m_log->printfln(DEBUG, "All 50 packets have been sent");
            /* We either move on to the next set
             * of dns hosts, or resolve the next host */
            packets_sent = 0;
            if(!m_waiting_to_resolve.empty()) {
                /* move on the the next address */
                to_resolve = m_waiting_to_resolve.front();
                m_waiting_to_resolve.pop();

                m_log->printfln(DEBUG, "Moving to address %s", to_resolve.c_str());
                return send_dns();
            } else {
                if(m_testing_invalid) {
                    /* we are done */
                    return send_results();
                } else {
                    m_testing_invalid = true;
                    m_latency_times = &m_invalid_latency_times;
                    enqueue_request_vector(conf.invalid_hostnames);
                    return send_dns();
                }
            }
        }
    }

    virtual void startTest(const TestConfig& conf, TestObserver* obs) {
        this->conf = conf;
        m_observer = obs;
    }

    void stop() {
        m_state_machine->stop();
    }

    void run() {
        m_state_machine->run();
    }


private:
    void calc_half_results(const vector<timeout_t>& vec,
                            f64_t& avg, f64_t& stdev, s64_t& lost) {
        timeout_t sum;
        timeout_t sumsq;
        size_t n = 0;
        lost = 0;

        vector<timeout_t>::const_iterator itr;
        FOR_EACH(itr, vec) {
            if(*itr == -1) {
                /* lost */
                ++ lost;
            } else {
                ++ n;
                sum += *itr;
                sumsq += (*itr) * (*itr);
            }
        }

        avg = ((f64_t)sum) / n;
        stdev = sqrt(1.0 / (n * (n-1)) * (n * sumsq - sum*sum));
    }
    State send_results() {
        TestResults to_send;
        calc_half_results(m_valid_latency_times,
            to_send.valid_avg_response_time_secs,
            to_send.valid_response_time_stdev,
            to_send.valid_packets_lost);

        calc_half_results(m_invalid_latency_times,
            to_send.invalid_avg_response_time_secs,
            to_send.invalid_response_time_stdev,
            to_send.invalid_packets_lost);

        if(m_observer) {
            m_observer->onTestComplete(to_send);
        }
        return IDLE;
    }

    void enqueue_request_vector(const vector<string>& hosts) {
        vector<string>::const_iterator itr;
        FOR_EACH(itr, hosts) {
            m_waiting_to_resolve.push(*itr);
        }
    }
    State send_dns() {
        byte packet[1024];
        size_t size = craft_dns_packet(packet, 1024);

        packets_sent ++;
        m_socket.sendTo(packet, size, *conf.server_address);
        m_state_machine->setTimeoutStim(5 SECS, TIMEOUT); /* 5 SEC timeout */
        sent_time = Time::currentTimeMicros();

        return PACKET_SENT;
    }

    size_t craft_dns_packet(byte* bytes, size_t len) {
        return 0;
    }
    /* The callback observer for the results of
     * the test */
    TestObserver* m_observer;

    /* Count of the number of dropped packets */
    size_t packets_sent;

    /* reference to either valid latency times or
     * invalid latency times */
    vector<timeout_t>* m_latency_times;

    vector<timeout_t> m_valid_latency_times;
    vector<timeout_t> m_invalid_latency_times;

    StateMachine<Stim, DnsTestImpl, State>* m_state_machine;

    /* BELOW is all stuff for the socket */

    /* main socket for communication */
    DatagramSocket m_socket;
    string to_resolve;
    TestConfig conf;

    /* queue of resolve requests */
    queue<string> m_waiting_to_resolve;
    bool m_testing_invalid; /* are we testing invalid names? */

    timeout_t sent_time;
};

TestProxy& Test::instance() {
    if(!m_instance) {
        DnsTestImpl* testimpl = new DnsTestImpl();
        testimpl->start();

        m_instance = new DnsTestImpl();
    }
    return *m_instance;
}

TestProxy* Test::m_instance;

}
