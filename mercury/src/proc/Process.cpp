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

class SubProcessThread : public Thread {
public:
    SubProcessThread(Runnable& r) :
        Thread(r) {}

    ~SubProcessThread(){
        m_threads_db[this] = NULL;
    }
};

Process::Process(const char* name) : name(name) {
    m_log = &LogManager::instance().getLogContext("Process", name);
}

FileCollection& Process::getFileCollection() {
    return m_file_collection;
}

Thread* Process::newThread(Runnable& runner) {
    Thread* ret = new SubProcessThread(runner);
    m_threads_db[ret] = this;
    return ret;
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
