
#include <io/Socket.hpp>
#include <io/ServerSocket.hpp>
#include <io/StringWriter.hpp>

#include <io/Inet4Address.hpp>

#include <io/binary/StreamGetter.hpp>

#include <cstdio>

#include <log/LogManager.hpp>

using namespace logger ;
using namespace std ;
using namespace io ;

int main( int argc, char** argv ) {
    (void) argc; (void) argv;

    LogContext& log = LogManager::instance().getLogContext("Main", "Main");
    log.setEnabled(true);

    log.printfln(TRACE, "simulating debug");
    log.printfln(DEBUG, "simulating debug");
    log.printfln(INFO, "logging has started");
    log.printfln(SUCCESS, "simulating success");
    log.printfln(WARN, "simulating a warning");
    log.printfln(ERROR, "This is simulating an error");

    return 0;
}
