#include <proc/Process.hpp>
#include <proc/ProcessManager.hpp>
#include <map>
#include <unistd.h>

using namespace std;
using namespace os;
using namespace io;

namespace proc {

ssize_t Terminal::read(byte* buf, size_t len) {
    return ::read(read_fd, buf, len);
}

ssize_t Terminal::write(const byte* buf, size_t len) {
    return ::write(write_fd, buf, len);
}

int Terminal::close() {
    return ::close(write_fd) | ::close(read_fd);
}

pair<Terminal, Terminal> Terminal::createTerminal() {
    int fd1[2];
    int fd2[2];

    pipe(fd1);
    pipe(fd2);

    Terminal t1(fd1[0], fd2[1]);
    Terminal t2(fd2[0], fd1[1]);

    return make_pair(t1, t2);
}

map<Thread*,Process*> m_threads_db;

class SubProcessThread : public Thread {
public:
    SubProcessThread(Runnable& r) :
        Thread(r) {}

    ~SubProcessThread(){
        m_threads_db[this] = NULL;
    }
};

Process::Process(const char* name) : name(name) {
    ProcessManager::instance().registerProcess(this);
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
    m_mesg_digest.super = this;
    Thread* dig = new Thread(m_mesg_digest);
    Thread* thr = new Thread(*this);

    dig->start();
    thr->start();

    return thr;
}

void Process::MessageDigester::run() {
    Message msg = super->inbound_messages.front();
    super->inbound_messages.pop();
    super->messageReceivedCallback(msg);
}

}
