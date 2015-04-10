#include <cstdio>

#include <io/Inet4Address.hpp>
#include <io/Resolv.hpp>
#include <io/ServerSocket.hpp>
#include <io/Socket.hpp>
#include <io/StringWriter.hpp>
#include <io/binary/StreamGetter.hpp>

#include <json/Json.hpp>
#include <json/JsonMercuryTempl.hpp>

#include <log/LogManager.hpp>

#include <mercury/Mercury.hpp>

#include <os/Condition.hpp>

#include <sys/wait.h>

#include <errno.h>
#include <signal.h>
#include <unistd.h>

using namespace logger ;
using namespace std ;
using namespace io ;
using namespace os ;
using namespace json ;

byte mercury_magic_cookie[32] = {
    0xe2, 0x1a, 0x14, 0xc8, 0xa2, 0x35, 0x0a, 0x92,
    0xaf, 0x1a, 0xff, 0xd6, 0x35, 0x2b, 0xa4, 0xf3, 
    0x97, 0x79, 0xaf, 0xb5, 0xc1, 0x23, 0x43, 0xf0,
    0xf7, 0x14, 0x17, 0x62, 0x53, 0x4a, 0xa9, 0x7e, 
};

class MercuryConfig {
public:
    MercuryConfig():
        logEverything(false),
        controller_url("http://127.0.0.1:5000/"),
        bind_ip(new Inet4Address("127.0.0.1", 8639)) {}

    bool logEverything;
    std::string controller_url;
    SocketAddress* bind_ip;
};

template<>
struct JsonBasicConvert<MercuryConfig> {
    static MercuryConfig convert(const json::Json& jsn) {
        MercuryConfig ret;
        ret.logEverything = jsn.hasAttribute("logEverything") && jsn["logEverything"] != 0;
        SocketAddress* old;

        if(jsn.hasAttribute("controller_url")) {
            ret.controller_url = jsn["controller_url"].stringValue();
        }
        if(jsn.hasAttribute("bind_ip")) {
            old = ret.bind_ip;
            ret.bind_ip = jsn["bind_ip"].convert<SocketAddress*>();
            delete old;
        }
        return ret;
    }
};

void sigchld_hdlr(int sig) {
}

int startMercury(LogContext& m_log, MercuryConfig& config) {
    StreamServerSocket sock;
    SocketAddress* bind_addr = config.bind_ip;

    m_log.printfln(INFO, "Binding to address: %s", bind_addr->toString().c_str());
    sock.bind(*bind_addr);
    sock.listen(1);

    while(true) {
        StreamSocket* client = sock.accept();
        m_log.printfln(INFO, "Connection accepted");

        /* simply get 64 bytes from the client and
         * then close the connection */
        byte recieve[64];
        client->read(recieve, sizeof(recieve));
        delete client;

        /* make sure the cookie is equal to what we expect */
        if(std::equal(recieve, recieve + 32, mercury_magic_cookie)) {
            /* the cookie is equal to what we expect
             * so continue with the fork() */
            m_log.printfln(INFO, "Magic cookie accepted, forking new process");
            pid_t child;
            if(!(child = fork())) {
                /* Child process. Setup the config and boot
                 * the merucry state machine */
                mercury::Config conf;
                std::copy(recieve + 32, recieve + 64, conf.mercury_id);
                conf.controller_url = config.controller_url;
                mercury::Proxy& process = mercury::ProxyHolder::instance();

                /* start mercury */
                process.start(conf, NULL);
            } else {
                /* parent */
                m_log.printfln(INFO, "Child started pid=%d", child);
                int res = 0;
                pid_t pid;

                /* Wait for the child to exit */
                if((pid = waitpid(child, &res, 0))) {
                    if(pid == -1) {
                        m_log.printfln(ERROR, "Error in waitpid %s", strerror(errno));
                    } else {
                        m_log.printfln(INFO, "Reap child (%d)\n", (int)pid);
                    }
                }

                if( WEXITSTATUS(res) ) {
                    m_log.printfln(WARN, "Child returned abnormal status: %d", WEXITSTATUS(res));
                }
            }
        } else {
            m_log.printfln(ERROR, "Bad magic cookie received. Timeout for 5 seconds");
            sleep(5);
            m_log.printfln(INFO, "Timeout complete");
        }
    }

    return 0;
}

int main( int argc, char** argv ) {
    (void) argc ; (void) argv ;

    MercuryConfig conf;
    signal(SIGCHLD, sigchld_hdlr);

    try {
        Json* jsn = Json::fromFile("config.json");
        conf = jsn->convert<MercuryConfig>();
    } catch(Exception& exp) {
        fprintf(stderr, "Unable to load config file: %s\n", exp.getMessage());
    };

    if(conf.logEverything) {
        LogManager::instance().logEverything();
    }

    LogContext& m_log = LogManager::instance().getLogContext("Main", "Main");
    m_log.printfln(INFO, "Mercury started");
    try {
        return startMercury(m_log, conf);
    } catch(Exception& exp) {
        m_log.printfln(FATAL, "Uncaught exception: %s", exp.getMessage());
        return 127;
    }
}
