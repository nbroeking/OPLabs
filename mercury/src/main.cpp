
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

using namespace mercury ;
using namespace logger ;
using namespace std ;
using namespace proc ;
using namespace io ;
using namespace os ;

int main( int argc, char** argv ) {
    (void) argc ; (void) argv ;

    try {
        LogManager::instance().logEverything();
        // LogManager::instance().setDefaultLevel(DEBUG);
        Mercury main_obj;
        Thread* thread = main_obj.start();
    
        thread->join();
    } catch ( Exception& e ) {
        logger::LogContext& log = logger::LogManager::instance().getLogContext("Main", "Main");       
        log.printfln(FATAL, "Terminate after throwing uncaught exception\n\t%s", e.getMessage());
    }

    return 0;
}
