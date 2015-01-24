
#include <io/Socket.hpp>
#include <io/ServerSocket.hpp>
#include <io/StringWriter.hpp>

#include <io/Inet4Address.hpp>

#include <io/binary/StreamGetter.hpp>

#include <cstdio>

#include <log/LogManager.hpp>
#include <proc/ProcessManager.hpp>
#include <unistd.h>

using namespace logger ;
using namespace std ;
using namespace proc ;
using namespace io ;
using namespace os ;

class MyProcess2: public Process {
public:
    MyProcess2() : Process("MyProcess2") {
    }

    void run() {
        LogContext& log = LogManager::instance().getLogContext("MyProcess2", "run");
        log.printfln(INFO, "MyProcess2 starting.");
        log.printfln(INFO, "Attempting to get a process and send a message");

        ProcessProxy* proxy = ProcessManager::instance().getProcessByName("MyProcess");
        if( ! proxy ) {
            log.printfln(ERROR, "Unable to find process.");
            return ;
        }

        const char* str = "ping";
        proxy->sendMessage(getAddress(), (byte*)str, strlen(str));

        sleep(50000000);
    }

protected:
    virtual void messageReceivedCallback( ProcessAddressProxy& from, const byte* bytes, size_t len ) {
        LogContext& log =
            LogManager::instance().getLogContext("MyProcess2", "MyProcess2");
        log.printfln(INFO, "Message received");
        log.printHex(INFO, bytes, len);
        from.sendMessage( getAddress(), (byte*)"ping", strlen("ping") );
    }
};

class MyProcess: public Process {
public:
    MyProcess() : Process("MyProcess") {
    }

    void run() {
        LogContext& log = LogManager::instance().getLogContext("MyProcess", "run");
        log.printfln(INFO, "MyProcess starting.");

        sleep(50000000);
    }

protected:
    virtual void messageReceivedCallback( ProcessAddressProxy& from, const byte* bytes, size_t len ) {
        LogContext& log =
            LogManager::instance().getLogContext("MyProcess", "MyProcess");
        log.printfln(INFO, "Message received");
        log.printHex(INFO, bytes, len);
        from.sendMessage( getAddress(), (byte*)"pong", strlen("pong") );
    }
};

int main( int argc, char** argv ) {
    (void) argc; (void) argv;

    LogManager::instance().setEnableByDefault(true);
    LogContext& log = LogManager::instance().getLogContext("Main", "Main");

    log.printfln(TRACE, "simulating debug");
    log.printfln(DEBUG, "simulating debug");
    log.printfln(INFO, "logging has started");
    log.printfln(SUCCESS, "simulating success");
    log.printfln(WARN, "simulating a warning");
    log.printfln(ERROR, "This is simulating an error");

    ProcessManager& man = ProcessManager::instance();
    log.printfln(INFO, "man: %p", &man);

    MyProcess process;
    Thread* m_thread = process.start();

    sleep(1);
    MyProcess2 process2;
    Thread* m_thread2 = process2.start();
    (void)m_thread2;

    m_thread->join();
    return 0;
}
