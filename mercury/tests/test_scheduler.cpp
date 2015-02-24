#include <os/Scheduler.hpp>

#include <cstdio>
#include <unistd.h>

using namespace std;
using namespace os;
using namespace logger;

class Runner: public Runnable {
public:
    Runner(const char* str, timeout_t off) {
        this->str = str;
        this->off = off;
    }
    void run() {
        printf("%s run at %f\n", str.c_str(), (Time::currentTimeMicros() - off) / 1000000.0 );
        fflush(stdout);
        delete this;
    }
private:
    timeout_t off;
    std::string str;
};

int main( int argc, char** argv ) {
    (void) argc; (void) argv;
    // LogManager::instance().logEverything();
    LogContext& log = LogManager::instance().getLogContext("Test", "Scheduler");
    log.setEnabled(true);
    log.printfln(INFO, "This is a test");


    Scheduler m_sched;
    Thread* m_sched_thread = m_sched.start();

    sleep(1);
    timeout_t off = Time::currentTimeMicros();
    m_sched.schedule(new Runner("Hi Josh", off), 3 SECS);

    for (int i = 0 ; i < 100 ; ++ i ) {
        char buf[128];
        timeout_t tm = rand() / 1000;
        snprintf(buf, sizeof(buf), "Task %03d at %f", i, (Time::relativeToAbsoluteTime(tm) - off) / 1000000.0);
        printf("%f\n", tm / 1000000.0);
        m_sched.schedule(new Runner(buf, off), tm );
    }

    if( !m_sched.verify_queue() ) {
        log.printfln(ERROR, "Heap verify fail");
    } else {
        log.printfln(SUCCESS, "Heap verify passed");
    }

    // sleep(10);
    m_sched.setStopOnEmpty(true);
    delete m_sched_thread;
}
