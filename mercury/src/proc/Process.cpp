#include <proc/Process.hpp>
#include <map>
#include <unistd.h>

using namespace std;
using namespace logger;
using namespace os;
using namespace io;

namespace proc {

map<Thread*,Process*> m_threads_db;

class SubProcessThread : public Thread {
public:
    SubProcessThread(Runnable& r) :
        Thread(r) {}

    ~SubProcessThread(){
        m_threads_db[this] = NULL;
    }
};

Process::Process(const char* name) : name(name),
    m_log(LogManager::instance().getLogContext("Process", name)) {
}

FileCollection& Process::getFileCollection() {
    return m_file_collection;
}

Thread* Process::newThread(Runnable& runner) {
    Thread* ret = new SubProcessThread(runner);
    m_threads_db[ret] = this;
    return ret;
}

Process* Process::getProcess() {
    Thread* me = Thread::getCurrentThread();
    return m_threads_db[me];
}

Thread* Process::start() {
    Thread* thr = new Thread(*this);

    thr->start();

    return thr;
}



}
