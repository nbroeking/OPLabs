#include <proc/Process.hpp>
#include <map>
#include <unistd.h>

using namespace std;
using namespace logger;
using namespace os;
using namespace io;

namespace proc {

map<Thread*,Process*> m_threads_db;
Process* main;

class MainProcess : public Process {
public:
    MainProcess(const char* name): Process(name) {}

    void run() OVERRIDE{
     /* Empty */   
    }
};

class _InternalThread : public Thread {
public:
    _InternalThread(ManagedRunnable& r) :
        Thread(r) {}

    ~_InternalThread(){
        m_threads_db[this] = NULL;
    }

    ManagedRunnable& getManagedRunnable() {
        return (ManagedRunnable&) getRunnable();
    }
};

Process::Process(const char* name) : name(name) {
    m_log = &LogManager::instance().getLogContext("Process", name);
    stopping = false;
    m_wait_mutex.lock();
}

FileCollection& Process::getFileCollection() {
    return m_file_collection;
}

Thread* Process::newThread(ManagedRunnable& runner) {
    _InternalThread* ret = new _InternalThread(runner);
    m_threads_db[ret] = this;
    m_threads.push_back(ret);

    return ret;
}

void Process::join() {
    if(stopping) return;

    stopping = true;
    vector<_InternalThread*>::iterator itr;

    m_log->printfln(INFO, "Stopping process %s", getName());

    /* Tell all the threads to stop */
    FOR_EACH(itr, m_threads) {
        if(*itr) {
            ManagedRunnable* mr = &(*itr)->getManagedRunnable();
            m_log->printfln(DEBUG, "Stopping runnable %p", mr);
            if(mr != this) mr->stop();
        }
    }

    FOR_EACH(itr, m_threads) {
        if(*itr) {
            m_log->printfln(DEBUG, "Join thread %p", *itr);
            ManagedRunnable* mr = &(*itr)->getManagedRunnable();
            if(mr != this) {
                delete (*itr);
                *itr = NULL;
            }
        }
    }

    m_threads.clear();
    m_log->printfln(DEBUG, "Process stopped");
}

void Process::stop() {
    join();
    m_wait_mutex.unlock();
}

Process& Process::getProcess() {
    Thread* me = Thread::getCurrentThread();
    map<Thread*, Process*>::iterator itr;
    itr = m_threads_db.find(me);

    if(itr == m_threads_db.end()) {
        if(main == NULL) {
            main = new MainProcess("Main");
        }
        return* main;
    }
    return* itr->second;
}

void Process::waitForExit() {
    ScopedLock __sl(m_wait_mutex);
}

Thread* Process::start() {
    Thread* thr = this->newThread(*this);
    thr->start();
    Thread* fc = this->newThread(getFileCollection());
    fc->start();
    m_sched = this->newThread(getScheduler());
    m_sched->start();

    m_thread = thr;
    return thr;
}



}
