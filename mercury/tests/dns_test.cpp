#include <mercury/dns/Test.hpp>
#include <log/LogManager.hpp>

#include <unistd.h>

using namespace io;
using namespace os;
using namespace json;
using namespace logger;

class Observer: public dns::TestObserver {
public:
    void onTestComplete(const dns::TestResults& res) OVERRIDE {
        (void)res;
        LogContext& log = LogManager::instance().getLogContext("Main", "Main");
        log.printfln(INFO, "Testing has completed");
        exit(0);
    }
};

int main(int argc, char** argv) {
    (void) argc; (void) argv;
    LogManager::instance().logEverything();
    LogContext& log = LogManager::instance().getLogContext("Main", "Main");

    dns::TestProxy& test = dns::Test::instance();
    dns::TestConfig conf;

    const char* json_blob =
        "{\n"
        "\"valid_names\": [\n"
        "    \"google.com\"\n"
        "],\n"
        "\"invalid_names\": [\n"
        "    \"potato.xyz\"\n"
        "],\n"
        "\"dns_server\": \"8.8.4.4:53\",\n"
        "\"timeout\": 1000\n"
        "}"
    ;
    Observer observer;
    log.printfln(INFO, "Parsing Json:\n%s", json_blob);

    try {
        uptr<Json> jsn = Json::parse(json_blob);
        conf = jsn->convert<dns::TestConfig>();
        test.startTest(conf, &observer);
        sleep(60);
        return 0;
    } catch(JsonException& excp) {
        log.printfln(ERROR, "Error parsing Json: %s", excp.getMessage());
    }
}
