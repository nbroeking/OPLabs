#include <proc/ProcessManager.hpp>
#include <io/UnixAddress.hpp>
#include <unistd.h>
#include <glob.h>
#include <io/binary/GlobPutter.hpp>
#include <os/ScopedLock.hpp>

using namespace std;
using namespace io;
using namespace os;
using namespace logger;

namespace proc {

inline std::vector<std::string> glob(const std::string& pat){
    using namespace std;
    glob_t glob_result;
    glob(pat.c_str(),GLOB_TILDE,NULL,&glob_result);
    vector<string> ret;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        ret.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return ret;
}

ProcessManager* s_procman_instance;


ProcessManager& ProcessManager::instance() {
    if( s_procman_instance == NULL )
        s_procman_instance = new ProcessManager();
    return *s_procman_instance;
}

ProcessProxy* ProcessManager::getProcessByName(const char* name) {
    ScopedLock __sl(m_mutex);
    return m_proc_table[m_proc_db[name]];
}

void ProcessManager::registerProcess(Process* p) {
    ScopedLock __sl(m_mutex);

    m_proc_db[p->getName()] = m_proc_table.size();
    m_proc_table.push_back(p);
}

void ProcessManager::observe( int fd, int events ) {
    (void)fd;
    (void)events;
    m_log.printfln( DEBUG, "Observing data from the world" );
    byte buf[1024];
    uptr<SocketAddress> addr;
    ssize_t bytes_read = m_outside.receive(buf, sizeof(buf), addr.cleanref());

    m_log.printfln( DEBUG, "Data from %s\n", addr==NULL?"Unspec":addr->toString().c_str() );
    m_log.printHex( DEBUG, buf, bytes_read );
}

ProcessManager::ProcessManager():
    m_log ( LogManager::instance().getLogContext("Process", "ProcessManager") ),
    m_addr("")
{
    pid_t pid = getpid();
    char name[1024];
    snprintf(name, sizeof(name), "/tmp/mercury.sock.%d", pid);

    m_addr.unlink();
    m_addr = UnixAddress(name);
    m_outside.bind(m_addr);

    m_filecollection.subscribeForRead(&m_outside, this);
    m_filecol_thread = new Thread(m_filecollection);
    m_filecol_thread->start();

    m_log.printfln( INFO, "ProcessManager started up. Listening on DatagramSocket @%s", m_addr.toString().c_str());
}

ProcessManager::~ProcessManager() {
  m_addr.unlink();
  discover();
}

void ProcessManager::discover() {
    ManagerPacket pkt;
    pkt.origin_pid = getpid();
    pkt.type = DISCOVER;
    io::GlobPutter p;
}

}

