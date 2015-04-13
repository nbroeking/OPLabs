#include <os/Scheduler.hpp>
#include <log/LogManager.hpp>

using namespace os;
using namespace logger;

void fn_runner(void *& v) {
    (void)v;
    printf("This is a thing %lu\n", Time::uptime());
}

int main() {
    LogManager::instance().logEverything();

    Scheduler sched;
    Thread* m_thread = new Thread(sched);
    m_thread->start();

    void* arg = NULL;
    for(int i = 0; i < 10; ++ i) {
        printf("Scheduling\n");
        sched.schedule(new FunctionRunner<void*>(fn_runner, arg), (i+1) SECS);
    }

    sched.setStopOnEmpty(true);
    m_thread->join();
    return 0;
}
