#include <mercury/dns/Test.hpp>
#include <io/DatagramSocket.hpp>
#include <io/DnsPacket.hpp>
#include <io/binary/GlobPutter.hpp>


#include <proc/StateMachine.hpp>
#include <proc/Process.hpp>

#include <cmath>
#include <missing.h>
#include <unistd.h>

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
        m_log->printfln(TRACE, "[%p] Created new state machine: %p", this, m_state_machine);
        m_state_machine->setEdge(IDLE, BEGIN_TEST, &DnsTestImpl::onBeginTest);
        m_state_machine->setEdge(PACKET_SENT, PACKET_RECEIEVED, &DnsTestImpl::onPacketReceieved);
        m_state_machine->setEdge(PACKET_SENT, TIMEOUT, &DnsTestImpl::onTimeout);

    }
    /* wait for observations in the DnsTestImpl */
    void observe(HasRawFd* fd, int events) OVERRIDE {
        (void) fd; (void) events;
        m_log->printfln(TRACE, "Observation in udp socket");

        byte bytes[1024];
        ssize_t bytes_read;

        bytes_read = m_socket.receive(bytes, sizeof(bytes), NULL);

        m_log->printfln(TRACE, "Packet receieved:");
        m_log->printHex(TRACE, bytes, bytes_read);

        m_state_machine->sendStimulus(PACKET_RECEIEVED);
    }

    State onBeginTest() {
        Inet4Address bind_addr("0.0.0.0", 0);
        m_log->printfln(DEBUG, "Binding to address: %s", bind_addr.toString().c_str());

        getFileCollection().unsubscribe(&m_socket);
        m_socket.bind(bind_addr);
        m_socket.setNonBlocking(true);
        getFileCollection().subscribe(FileCollection::SUBSCRIBE_READ, &m_socket, this);

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

    State move_along() {
        if(packets_sent < 1) {
            /* we send 50 dns requests for each
             * host to resolve */
            return send_dns();
        } else {
            m_log->printfln(DEBUG, "All packets have been sent");
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
                    m_log->printfln(INFO, "Finished running test. Sending results.");
                    return send_results();
                } else {
                    m_testing_invalid = true;
                    m_latency_times = &m_invalid_latency_times;
                    enqueue_request_vector(conf.invalid_hostnames);

                    m_log->printfln(INFO, "Move to testing invalid");
                    to_resolve = m_waiting_to_resolve.front();
                    m_waiting_to_resolve.pop();
                    return send_dns();
                }
            }
        }
    }

    State onTimeout() {
        m_latency_times->push_back(-1);
        return move_along();
    }

    State onPacketReceieved() {
        /* Add to the latency times */
        timeout_t to = Time::currentTimeMicros();
        m_log->printfln(INFO, "Packet %d. Latency %f ms", packets_sent, (to - sent_time)/1000.0);
        m_latency_times->push_back(to - sent_time);
        return move_along();
    }

    virtual void startTest(const TestConfig& conf, TestObserver* obs) {
        this->conf = conf;
        m_observer = obs;

        m_log->printfln(INFO, "[%p] Sending stimulus BEGIN_TEST", this);
        m_state_machine->sendStimulus(BEGIN_TEST);
    }

    void stop() {
        Process::stop();
    }

    void run() {
        m_log->printfln(DEBUG, "Starting DnsTestImpl");
        m_log->printfln(TRACE, "Running state machine %p", m_state_machine);
        m_state_machine->run();
    }


private:
    void calc_half_results(const vector<timeout_t>& vec,
                            f64_t& avg, f64_t& stdev, s64_t& lost) {
        timeout_t sum = 0;
        timeout_t sumsq = 0;
        size_t n = 0;

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

        m_log->printfln(TRACE, "SUM: %lu. N: %lu\n", (lu_t)sum, (lu_t)n);
        avg = ((f64_t)sum) / n;
        stdev = sqrt(1.0 / (n * (n-1)) * (n * sumsq - sum*sum));
    }
    State send_results() {
        TestResults to_send;
        s64_t lost = 0;
        calc_half_results(m_valid_latency_times,
            to_send.valid_avg_response_time_mircos,
            to_send.valid_response_time_stdev,
            lost);

        calc_half_results(m_invalid_latency_times,
            to_send.invalid_avg_response_time_mircos,
            to_send.invalid_response_time_stdev,
            lost);

        to_send.packets_lost_ratio = ((f64_t)lost) / (m_valid_latency_times.size() + m_invalid_latency_times.size());

        m_log->printfln(DEBUG, "Results: %f %f | %f %f | %f",
            to_send.valid_avg_response_time_mircos,
            to_send.valid_response_time_stdev,

            to_send.invalid_avg_response_time_mircos,
            to_send.invalid_response_time_stdev,
            to_send.packets_lost_ratio);

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
        byte* packet;
        ssize_t packet_size;


        packet = craft_dns_packet(to_resolve, rand(), packet_size);

        m_log->printfln(TRACE, "Crafted DNS packet to send:");
        m_log->printHex(TRACE, packet, packet_size);

        packets_sent ++;

        m_log->printfln(INFO, "Sending DNS to %s for %s", conf.server_address->toString().c_str(), to_resolve.c_str());
        m_socket.sendTo(packet, packet_size, *conf.server_address);
        m_state_machine->setTimeoutStim(conf.timeout_micros, TIMEOUT); /* 5 SEC timeout */
        sent_time = Time::currentTimeMicros();

        delete[] packet;

        return PACKET_SENT;
    }

    byte* craft_dns_packet(const std::string& hostname, short dnsid, ssize_t& sizeout) {
        DnsPacket packet(hostname.c_str(), dnsid);
        GlobPutter putter;
        putObject(putter, packet);

        return putter.serialize(sizeout);
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

using namespace logger;
TestProxy& Test::instance() {
    LogContext& log = LogManager::instance().getLogContext("Main", "Main");
    log.printfln(TRACE, "Test::instance)");

    if(!m_instance) {
        DnsTestImpl* testimpl = new DnsTestImpl();
        log.printfln(TRACE, "Starting new dns test impl");
        testimpl->start();

        m_instance = testimpl;
    }
    return *m_instance;
}

TestProxy* Test::m_instance;

}
