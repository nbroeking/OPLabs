#include <mercury/ping/Test.hpp>
#include <log/LogManager.hpp>

#include <unistd.h>

using namespace io;
using namespace os;
using namespace logger;

class Observer: public ping::TestObserver {
public:
    void onTestComplete(const ping::TestResults res) OVERRIDE {
        LogContext& log = LogManager::instance().getLogContext("Main", "Main");
        log.printfln(INFO, "Testing has completed");
    }
};

int main(int argc, char** argv) {
    LogManager::instance().logEverything();
    LogContext& log = LogManager::instance().getLogContext("Main", "Main");

    ping::TestProxy& test = ping::Test::instance();
    ping::TestConfig conf;

    conf.ping_addrs.push_back(new Inet4Address("8.8.8.8", 0));
    conf.ping_addrs.push_back(new Inet4Address("8.8.4.4", 0));
    conf.ping_addrs.push_back(new Inet4Address("93.184.216.34", 0));
    conf.ping_addrs.push_back(new Inet4Address("1.2.3.4", 0));

    test.start(conf, new Observer());

    sleep(6);
    return 0;
}
