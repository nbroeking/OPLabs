#include <os/Scheduler.hpp>

#include <cstdio>
#include <unistd.h>
#include <test/Tester.hpp>

using namespace std;
using namespace os;
using namespace logger;
using namespace containers;
using namespace test;

class Runner: public Runnable {
public:
    Runner(const char* str, timeout_t time, timeout_t off, BlockingQueue<timeout_t>& runtimes):
        runtimes(runtimes){
        this->str = str;
        this->off = off;
        this->time = time;
    }
    void run() {
        ScopedLock __sl(s_mutex);

        timeout_t rt = Time::currentTimeMicros() - off;
        printf("%s run at %f time=%f (+%f)\n", str.c_str(), rt/10.0e6, time/10.0e6, (rt-time)/10e6);
        runtimes.push(time);
        fflush(stdout);
        delete this;
    }

    static Mutex s_mutex;
private:
    timeout_t off;
    timeout_t time;
    std::string str;
    BlockingQueue<timeout_t>& runtimes;
};
Mutex Runner::s_mutex;

class ThreadRunner : public Runnable {
public:
    ThreadRunner(Scheduler& sched, timeout_t off, BlockingQueue<timeout_t>& toqueue):
        toqueue(toqueue),
        sched(sched),
        off(off),
        log(LogManager::instance().getLogContext("Test", "Scheduler")){
        }

    void run() {

        Time::sleep(rand() % 100);
        for(int i = 0; i < 10; ++ i) {
            char buf[128];
            timeout_t tm = rand() / 1000;
            timeout_t when = Time::relativeToAbsoluteTime(tm) - off;
            snprintf(buf, sizeof(buf), "Task %03d at %f", i, when/10.0e6);
            log.printfln(INFO, "%f\n", tm / 1000000.0);
            sched.schedule(new Runner(buf, when, off, toqueue), tm);
        }
    }


private:
    BlockingQueue<timeout_t>& toqueue;
    Scheduler& sched;
    timeout_t off;
    LogContext& log;
};

/* Test to see if the queue is sorted within reasonable limits */
bool is_sorted(BlockingQueue<timeout_t>& bq) {
    LogContext& log = LogManager::instance().getLogContext("Test", "Scheduler");
    timeout_t last = 0;
    while(!bq.empty()) {
        timeout_t cur = bq.front();
        bq.pop();
        if(cur < last && (last - cur) > 5 MILLIS ) {
            /* we give a 5 ms grace period */
            log.printfln(ERROR, "%ld is less than %ld", cur, last);
            return false;
        }
        last = cur;
    }
    return true;
}

int main( int argc, char** argv ) {
    (void) argc; (void) argv;
    Tester::init("Scheduler", argc, argv);

    LogManager::instance().logEverything();
    LogManager::instance().setDefaultLevel(DEBUG);

    // LogManager::instance().logEverything();
    LogContext& log = LogManager::instance().getLogContext("Test", "Scheduler");
    BlockingQueue<timeout_t> run_times; 
    log.setEnabled(true);
    log.printfln(INFO, "This is a test");

    srand(12345);

    Scheduler m_sched;
    Thread* m_sched_thread = new Thread(m_sched);
    m_sched_thread->start();

    timeout_t off = Time::currentTimeMicros();
    m_sched.schedule(new Runner("Hi Josh", 3 SECS, off, run_times), 3 SECS);

    ThreadRunner m_thread_runner(m_sched, off, run_times);

    {   uptr<Thread> m_threads[10];
        for (int i = 0 ; i < 10 ; ++ i ) {
            m_threads[i] = new Thread(m_thread_runner);
            m_threads[i]->start();
        }
    }


    TEST_BOOL("Heap Verify", m_sched.verify_queue());

    // sleep(10);
    m_sched.setStopOnEmpty(true);
    /* join and delete the thread */
    delete m_sched_thread;

    /* The jobs should have been run in order */
    TEST_BOOL("Runtimes Sorted", is_sorted(run_times));

    Tester::instance().exit();
}
