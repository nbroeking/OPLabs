#ifndef MERCURY_HPP_
#define MERCURY_HPP_

/*
 * Author: jrahm
 * created: 2015/02/12
 * Mercury.hpp: <description>
 */

#include <proc/Process.hpp>
#include <proc/StateMachine.hpp>

#include <log/LogContext.hpp>
#include <io/ServerSocket.hpp>
#include <curl/AsyncCurl.hpp>

namespace mercury {

enum MercuryStim {
      COOKIE_RECEIVED
    , BAD_COOKIE_RECEIVED /* cause pause */
    , BAD_REQUEST /* unsolicited test request; cause pause */
    , GOOD_REQUEST
    , TESTS_COMPLETE
    , RESULT_POST_ACK

    , N_MERC_STIMS
};

enum MercuryState {
      IDLE /* initial state */
    , REQUEST_MADE /* curl request was sent */
    , PAUSE /* wait some time before starting up */
    , TEST_STARTED /* stimulate some test state machine */
    , RESULTS_POSTED

    , N_MERC_STATES
};

inline std::string toString( MercuryStim stim ) {
    static const char* names[] = {
        "CookieReceived", "BadCookieReceived",
        "BadRequest", "GoodRequest", "TestsComplete",
        "ResultPostAck"
    };
    return stim < N_MERC_STIMS ? names[stim] : "Unknown";
}


inline std::string toString( MercuryState state ) {
    static const char* state_names[] = {
        "Idle", "RequestMade", "Pause",
        "TestStarted", "ResultsPosted"
    };
    return state < N_MERC_STATES ? state_names[state] : "Unknown";
}

class Mercury: public proc::Process {
public:
    Mercury();

    void run() OVERRIDE;

    void setupStateMachine() {
        m_state_machine.setEdge(IDLE, COOKIE_RECEIVED,
            &Mercury::onCookieReceived);

        m_state_machine.setEdge(IDLE, BAD_COOKIE_RECEIVED,
            &Mercury::onIncorrectCookie);

        m_state_machine.setEdge(REQUEST_MADE, BAD_REQUEST,
            &Mercury::onBadRequest);

        m_state_machine.setEdge(REQUEST_MADE, GOOD_REQUEST,
            &Mercury::onGoodRequest);
    }

    /* State machine implementation */
    MercuryState onCookieReceived();
    MercuryState onIncorrectCookie();
    MercuryState onBadRequest();
    MercuryState onGoodRequest();

private:

    friend class AcceptHandler;
    friend class SocketHandler;
    friend class MercuryCurlObserver;

    class MercuryCurlObserver: public curl::CurlObserver {
    public:
        void onException( curl::CurlException& excp ) {
            m_sup->m_state_machine.sendStimulus( BAD_REQUEST );
        }
        void onOK() {
            m_sup->m_state_machine.sendStimulus( GOOD_REQUEST );
        }
        void read( const byte* bytes, size_t len ) {
            for( size_t i = 0; i < len ; ++ i )
                m_sup->m_buffer_data.push_back(bytes[i]);
        }
        Mercury* m_sup;
    };

    /* Class that handles a single socket connection using
     * the file collection interface */
    class SocketHandler: public io::FileCollectionObserver {
    public:
        SocketHandler( io::StreamSocket* sock, Mercury* sup );
        ~SocketHandler();
        io::StreamSocket* m_sock;
        Mercury* m_sup;
        virtual void observe(int fd, int events);
    };

    /* Handler that accepts connections from a server
     * socket using the File collection interface */
    class AcceptHandler: public io::FileCollectionObserver {
    public:
        Mercury* m_sup;
        virtual void observe(int fd, int events);
    };

    curl::AsyncCurl m_asnc_curl;

    /* The handler for the server socket */
    AcceptHandler m_accept_handler;

    /* The internal state machine */
    StateMachine<MercuryStim, Mercury, MercuryState>
        m_state_machine;

    /* The server socket that this is listening on */
    io::StreamServerSocket m_sock;

    /* my log */
    logger::LogContext& m_log;

    std::vector<byte> m_buffer_data;
};

}

#endif /* MERCURY_HPP_ */
