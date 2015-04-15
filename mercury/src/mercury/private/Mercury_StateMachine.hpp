#ifndef MERCURY_STATEMACHINE_HPP_
#define MERCURY_STATEMACHINE_HPP_

/*
 * Author: jrahm
 * created: 2015/03/26
 * Mercury_StateMachine.hpp: <description>
 */

#include <log/LogManager.hpp>
#include <curl/AsyncCurl.hpp>

#include <io/binary/Base64Putter.hpp>
#include <io/binary/GlobPutter.hpp>

#include <proc/StateMachine.hpp>
#include "Mercury_JSON.hpp"

#include <mercury/dns/Test.hpp>
#include <mercury/throughput/Test.hpp>
#include <mercury/Mercury.hpp>


using namespace json;
using namespace logger;
using namespace curl;
using namespace std;
using namespace io;

namespace mercury {

enum State {
    IDLE,
    REQUEST_MADE,
    TIMEOUT,
    DNS_TEST_RUNNING,
    POSTING_DNS_RESULTS,
    THROUGHPUT_RUNNING,
    POSTING_THROUGHPUT_RESULTS,
};

enum Stim {
    START,
    BAD_REQUEST,
    GOOD_REQUEST,
    WAIT_TIMEOUT,
    DNS_TEST_FINISHED,
    THROUGHPUT_FINISHED,
    TIMEOUT_STIM
};

ENUM_TO_STRING(State, 6,
    "Idle", "RequestMade",
    "Timeout", "DnsTestStarted",
    "PostingDnsResults", "ThroughputRinning",
    "PostingThroughputResults"
    )

ENUM_TO_STRING(Stim, 7,
    "Start",
    "BadRequest", "GoodRequest",
    "WaitTimeout", "DnsTestFinished",
    "ThroughputFinished", "Timeout"
    )

#define ID_SIZE 32

inline string html_escape(const string& in) {
    string::const_iterator itr;
    string out;

    for( itr = in.begin() ; itr != in.end() ; ++ itr ) {
        if( *itr == '+' ) {
            out += "%2b";
        } else {
            out.push_back( *itr );
        }
    }

    return out;
}

class Mercury;
class MercuryObserver: public CurlObserver,
                       public dns::TestObserver,
                       public throughput::TestObserver,
                       public os::ManagedRunnable {
    /* Interface */
};

void setup_curl(Curl& c, const char* url, const char* post_data) {
    c.setURL(url);
    c.setPostFields(post_data);
    c.setFollowLocation(true);
    c.setSSLHostVerifyEnabled(false);
    c.setSSLPeerVerifyEnabled(false);
}

class Mercury_StateMachine {
public:

Mercury_StateMachine(MercuryObserver* observer):
    m_log(LogManager::instance().getLogContext("Mercury", "StateMachine")) {
    m_observer = observer;
}

void curlSendJson(Json to_send) {
    char post_chars[4096];
    Json tmp = Json::fromString("finished");
    to_send.setAttribute("status", tmp);
    snprintf(post_chars, sizeof(post_chars), "data=%s&router_token=%s",
        to_send.toString().c_str(), m_id_b64.c_str());
    Curl request;
    m_post_fields = post_chars;
    m_current_url = m_config.controller_url + "/api/router/edit";

    m_log.printfln(DEBUG, "Sending results %s", post_chars);
    setup_curl(request, m_current_url.c_str(), m_post_fields.c_str());
    m_async_curl.sendRequest(request, m_observer);
}

void sendDnsResults(const dns::TestResults& res) {
    using namespace json;

    Json to_send = Json::from(res);
    curlSendJson(to_send);
}

State onStart() {
    /* On the start, we have to get the configuration from the
     * state machine */
    m_log.printfln(INFO, "Starting mercury");
    /* The if of this router */
    m_id_b64 = simpleBase64Encode(m_config.mercury_id, ID_SIZE);
    m_id_b64 = html_escape(m_id_b64);
    m_log.printfln(DEBUG, "base64 encoded id: %s", m_id_b64.c_str());

    Curl request;

    m_post_fields = string("router_token=") + m_id_b64;
    m_current_url = m_config.controller_url + "/api/router/get_config";
    setup_curl(request, m_current_url.c_str(), m_post_fields.c_str());

    m_log.printfln(INFO, "sending curl request with data %s", m_post_fields.c_str());
    m_async_curl.sendRequest(request, m_observer);

    m_state_machine->setTimeoutStim(5 SECS, TIMEOUT_STIM);
    return REQUEST_MADE;
}

State onDnsComplete() {
    m_state_machine->setTimeoutStim(5 SECS, TIMEOUT_STIM);
    return POSTING_DNS_RESULTS;
}

State onGoodRequest() {
    m_log.printfln(SUCCESS, "Good request made. Returned string %s", m_response);
    /* do the json stuff */

    bool fail;

    try {
        m_test_conf = parseMercuryTestConfig((char*)m_response, fail);
    } catch(json::JsonException& ex) {
        m_log.printfln(ERROR, "Error parsing JSON request: %s", ex.getMessage()); return IDLE;
    }

    if(fail) {
        m_log.printfln(ERROR, "Server returned failure status. Timeout.");
        m_state_machine->setTimeoutStim(5 SECS, WAIT_TIMEOUT);
        return TIMEOUT;
    }

    m_log.printfln(SUCCESS, "Good configuration response");

    /* build the configuration and start the
     * test */
    dns::Test::instance().startTest(m_test_conf.dns_test_config, m_observer);

    /* Start the ping test */
    return DNS_TEST_RUNNING;
}

State onBadRequest() {
    m_log.printfln(WARN, "Bad request made.");
    m_log.printHex(DEBUG, m_response, m_response_len);
    m_state_machine->setTimeoutStim(5 SECS, WAIT_TIMEOUT);
    return TIMEOUT;
}

State onTimeout() {
    m_log.printfln(WARN, "Timeout in %s", toString(m_state_machine->getCurrentState()).c_str());
    exit(3);
}

State exitSoftly() {
    exit(0);
}

State exitHard() {
    m_log.printfln(DEBUG, "Bad response:");
    m_log.printHex(DEBUG, m_response, m_response_len);
    exit(1);
}


State onThroughputTestFinished() {
    using namespace json;
    Json to_send = Json::from(m_throughput_test_results);
    curlSendJson(to_send);

    m_state_machine->setTimeoutStim(5 SECS, TIMEOUT_STIM);
    return POSTING_THROUGHPUT_RESULTS;
}

State onDnsResultsPosted() {
    throughput::TestProxy& proxy =
            throughput::Test::instance();

    m_log.printfln(INFO, "Running throughput test");
    proxy.startTest(m_test_conf.throughput_test_config, m_observer); /* TODO NOT NULL */
    m_state_machine->setTimeoutStim(20 SECS, TIMEOUT_STIM);
    return THROUGHPUT_RUNNING;
}

State onThroughputResultsPosted() {
    m_log.printfln(INFO, "No more tests to run. Teardown");
    m_observer->stop();
}

State onWaitTimeoutComplete() {
    m_log.printfln(INFO, "Timeout complete");
    exit(1);

    return IDLE; /* shouldn't return */
}

State onPingTestFinished() {
    m_log.printfln(INFO, "Ping test complete. Sending results");
    /* TODO: Send results */
    return IDLE;
}
    

private:

/* these two classes really operate as the
 * same class, but for clarity are broken into
 * separate ones. */
friend class Mercury;

/* This will be an instance of mercury and will
 * stimulate the state machine */
MercuryObserver* m_observer;

/* The log for this state machine */
LogContext& m_log;

AsyncCurl m_async_curl;

ssize_t m_response_len;
byte* m_response;
StateMachine<Stim, Mercury_StateMachine, State>* m_state_machine;

std::string m_current_url; /* For garbage collection */
std::string m_post_fields;
std::string m_id_b64;
Config m_config;
MercuryTestConfig m_test_conf;

throughput::TestResults m_throughput_test_results;
};

}

#endif /* MERCURY_STATEMACHINE_HPP_ */
