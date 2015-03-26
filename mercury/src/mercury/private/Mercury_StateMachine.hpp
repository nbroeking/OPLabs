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


using namespace logger;
using namespace curl;
using namespace std;
using namespace io;

namespace mercury {

enum State {
    IDLE,
    REQUEST_MADE
};

enum Stim {
    COOKIE_RECEIVED,
    BAD_COOKIE_RECEIVED,
    BAD_REQUEST,
    GOOD_REQUEST
};

ENUM_TO_STRING(State, 2,
    "Idle", "RequestMade")

ENUM_TO_STRING(Stim, 4,
    "CookieReceived", "BadCookieReceived",
    "GoodRequest", "BadRequest")

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
class MercuryObserver: public CurlObserver {
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

State onMagicCookieReceived() {
    m_log.printfln(INFO, "Magic cookie received");
    /* The if of this router */
    string id_enc = simpleBase64Encode(m_id, ID_SIZE);
    id_enc = html_escape(id_enc);
    m_log.printfln(DEBUG, "base64 encoded id: %s", id_enc.c_str());

    Curl request;
    char post_data[128];
    snprintf(post_data, sizeof(post_data), "id=%s", id_enc.c_str());
    setup_curl(request, "https://128.138.202.143/api/start_test", post_data);

    m_log.printfln(INFO, "sending curl request with data %s", post_data);
    m_async_curl.sendRequest(request, m_observer);

    return REQUEST_MADE;
}

State onBadCookie() {
    m_log.printfln(WARN, "Bad magic cookie found");
    return IDLE;
}
    

private:
friend class Mercury;

/* This will be an instance of mercury and will
 * stimulate the state machine */
MercuryObserver* m_observer;

/* The log for this state machine */
LogContext& m_log;
byte m_id[ID_SIZE];

AsyncCurl m_async_curl;

ssize_t m_response_len;
byte* m_response;
};

}

#endif /* MERCURY_STATEMACHINE_HPP_ */
