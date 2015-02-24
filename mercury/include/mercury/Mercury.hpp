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

#define SERVER_ID_LENGTH 32
#define MAGIC_COOKIE_LENGTH 32

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

struct ConfigPacket {
    /* Ookla, Ping, Dns */

    std::vector< uptr<io::SocketAddress> > ookla_address;
    std::vector< uptr<io::SocketAddress> > ping_address;
    std::vector< uptr<io::SocketAddress> > dns_address;
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


extern byte mercury_magic_cookie[ MAGIC_COOKIE_LENGTH ];

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


    inline void setId( byte* bytes /* MUST BE LENGTH 32 */ ) {
        m_log.printfln(DEBUG, "Setting new id to");
        m_log.printHex(DEBUG, bytes, SERVER_ID_LENGTH);
        std::copy(bytes, bytes + SERVER_ID_LENGTH, m_id);
    }
private:

    friend class AcceptHandler;
    friend class SocketHandler;
    friend class MercuryCurlObserver;

    class MercuryCurlObserver: public curl::CurlObserver {
    public:
        void onException( curl::CurlException& excp ) {
            m_sup->m_log.printfln(ERROR, "Error with curl request: %s", excp.getMessage());
            m_sup->m_state_machine.sendStimulus( BAD_REQUEST );
        }
        void onOK(curl::http_response_code_t code) {
            m_sup->m_log.printfln(INFO, "Curl status code returned %d", code);

            if( code == 200 ) {
                /* Only send a good stim on success */
                m_sup->m_state_machine.sendStimulus( GOOD_REQUEST );
            } else {
                m_sup->m_state_machine.sendStimulus( BAD_REQUEST );
            }
        }
        void read( const byte* bytes, size_t len ) {
            for( size_t i = 0; i < len ; ++ i )
                m_sup->m_buffer_data.push_back(bytes[i]);
        }
        Mercury* m_sup;
    };

    int parseConfigPacket(ConfigPacket& cfg);
    void log_config_pkt(ConfigPacket& pkt);

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

    byte m_id[ SERVER_ID_LENGTH ]; /* server side identification */

    proc::ProcessProxy* m_ping_test;

    ConfigPacket m_configuration;
};

}

#endif /* MERCURY_HPP_ */
