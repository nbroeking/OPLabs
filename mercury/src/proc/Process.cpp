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

void Process::stop() {
    vector<_InternalThread*>::iterator itr;

    /* Tell all the threads to stop */
    FOR_EACH(itr, m_threads) {
        (*itr)->getManagedRunnable().stop();
    }

    FOR_EACH(itr, m_threads) {
        delete (*itr);
    }

    m_threads.clear();
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

Thread* Process::start() {
    Thread* thr = this->newThread(*this);
    thr->start();
    Thread* fc = this->newThread(getFileCollection());
    fc->start();
    Thread* sched = this->newThread(getScheduler());
    sched->start();

    return thr;
}



}
