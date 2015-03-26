
#include <io/Socket.hpp>
#include <io/ServerSocket.hpp>
#include <io/StringWriter.hpp>

#include <io/Inet4Address.hpp>
#include <io/Resolv.hpp>

#include <io/binary/StreamGetter.hpp>

#include <cstdio>

#include <log/LogManager.hpp>
#include <unistd.h>

#include <mercury/Mercury.hpp>
#include <os/Condition.hpp>

using namespace logger ;
using namespace std ;
using namespace io ;
using namespace os ;

class MercuryCallback : public mercury::Callback {
public:
    void onStop() {
        ScopedLock __sl(mut);
        cond.signal();
    }
    Condition cond;
    Mutex mut;
};

int main( int argc, char** argv ) {
    (void) argc ; (void) argv ;

    MercuryCallback callback;
    ScopedLock __sl(callback.mut);

    try {
        LogManager::instance().logEverything();
        // LogManager::instance().setDefaultLevel(DEBUG);
        mercury::Proxy& proxy = mercury::ProxyHolder::instance();
        mercury::Config conf;
        proxy.start(conf, &callback);

        /* wait for exit */
        callback.cond.wait(callback.mut);

    } catch ( Exception& e ) {
        logger::LogContext& log = logger::LogManager::instance().getLogContext("Main", "Main");       
        log.printfln(FATAL, "Terminate after throwing uncaught exception\n\t%s", e.getMessage());
    }

    return 0;
}
