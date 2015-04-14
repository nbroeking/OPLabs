#ifndef PRIVATE_MERCURY_HPP_
#define PRIVATE_MERCURY_HPP_

/*
 * Author: jrahm
 * created: 2015/03/26
 * Mercury.hpp: <description>
 */

#include "Mercury_StateMachine.hpp"

#include <proc/StateMachine.hpp>
#include <proc/Process.hpp>

#include <io/Inet4Address.hpp>
#include <os/Atomic.hpp>

using namespace proc;
using namespace json;
using namespace os;

namespace mercury {
byte mercury_magic_cookie[32] = {
    0xe2, 0x1a, 0x14, 0xc8, 0xa2, 0x35, 0x0a, 0x92,
    0xaf, 0x1a, 0xff, 0xd6, 0x35, 0x2b, 0xa4, 0xf3, 
    0x97, 0x79, 0xaf, 0xb5, 0xc1, 0x23, 0x43, 0xf0,
    0xf7, 0x14, 0x17, 0x62, 0x53, 0x4a, 0xa9, 0x7e, 
};

class Mercury: public MercuryObserver, public Process {
public:
    Mercury():
        Process("Mercury"),
        m_mercury_state_machine(this),
        m_state_machine(NULL),
        m_log(LogManager::instance().getLogContext("Process", "Mercury")),
        m_response(NULL){
        }

    /* File collection observer */
    void read(const byte* bytes, size_t len) {
        m_response_putter.putBytes(bytes, len);
    }

    void setConfig(const Config& conf) {
        m_mercury_state_machine.m_config = conf;
    }

    /* CURL Asyc Observer */
    void onOK(http_response_code_t status_code) {
        m_log.printfln(INFO, "Curl returned status %d", status_code);

        delete[] m_response;
        m_response_putter.putByte(0);
        m_response = m_response_putter.serialize(m_response_len);
        m_response_putter.clear();
        m_mercury_state_machine.m_response = m_response;
        m_mercury_state_machine.m_response_len = m_response_len;

        if(status_code == 200) {
            m_state_machine->sendStimulus(GOOD_REQUEST);
        } else {
            m_state_machine->sendStimulus(BAD_REQUEST);
        }
    }

    void onException(CurlException& expt) {
        m_log.printfln(ERROR, "Error with curl request: %s", expt.getMessage());
        m_state_machine->sendStimulus(BAD_REQUEST);
    }
    
    /* Implement ping test observer */
    void onTestComplete(const dns::TestResults& res) {
        m_log.printfln(DEBUG, "Test results in avg_latency=%fs; packets_lost=%f",
            res.valid_avg_response_time_mircos/1e6, res.packets_lost_ratio);

        m_mercury_state_machine.sendDnsResults(res);
        m_state_machine->sendStimulus(DNS_TEST_FINISHED);
    }
    void run() OVERRIDE {
        ScopedLock __sl(m_mutex);
        _run();
    }

    void exit() {
        m_exit_cond.signal();
    }
private:
    void setup_state_machine() {
        m_state_machine->setEdge(IDLE, START, &Mercury_StateMachine::onStart);

        m_state_machine->setEdge(REQUEST_MADE, GOOD_REQUEST, &Mercury_StateMachine::onGoodRequest);
        m_state_machine->setEdge(REQUEST_MADE, BAD_REQUEST, &Mercury_StateMachine::onBadRequest);
        m_state_machine->setEdge(REQUEST_MADE, TIMEOUT_STIM, &Mercury_StateMachine::onTimeout);

        m_state_machine->setEdge(TIMEOUT, WAIT_TIMEOUT, &Mercury_StateMachine::onWaitTimeoutComplete);
        m_state_machine->setEdge(DNS_TEST_RUNNING, DNS_TEST_FINISHED, &Mercury_StateMachine::onDnsComplete);

        m_state_machine->setEdge(POSTING_DNS_RESULTS, GOOD_REQUEST, &Mercury_StateMachine::onDnsResultsPosted);
        m_state_machine->setEdge(POSTING_DNS_RESULTS, BAD_REQUEST, &Mercury_StateMachine::onBadRequest);

        newThread(*m_state_machine)->start();
    }

    void _run() {
        m_log.printfln(INFO, "Starting mercury");
        m_state_machine = new StateMachine<Stim, Mercury_StateMachine, State>
                                (m_mercury_state_machine, IDLE, getScheduler());
        m_mercury_state_machine.m_state_machine = m_state_machine;
        setup_state_machine();

        Thread* th = newThread(m_mercury_state_machine.m_async_curl);
        th->start();
        m_state_machine->sendStimulus(START);

        wait_for_exit();
    }

    void wait_for_exit() {
        m_exit_cond.wait(m_mutex);
    }

    Mutex m_mutex;
    Condition m_exit_cond;
    Mercury_StateMachine m_mercury_state_machine;
    StateMachine<Stim, Mercury_StateMachine, State>* m_state_machine; /* Construct this when we run */
    StreamServerSocket m_server_sock;
    LogContext& m_log;


    GlobPutter m_response_putter;
    byte* m_response;
    ssize_t m_response_len;
};

}

#endif /* MERCURY_HPP_ */
