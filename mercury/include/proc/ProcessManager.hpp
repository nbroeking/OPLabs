#ifndef PROCESSMANAGER_HPP_
#define PROCESSMANAGER_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * ProcessManager.hpp: <description>
 */

#include <proc/Process.hpp>
#include <io/DatagramSocket.hpp>
#include <os/Mutex.hpp>
#include <map>
#include <log/LogManager.hpp>
#include <io/UnixAddress.hpp>
#include <io/binary/Putter.hpp>

namespace proc {

enum PacketType {
    SEND = 0,
    DISCOVER,
    DISCOVER_RESPONSE
};

class ManagerPacket {
public:
    uint32_t origin_pid;
    PacketType type;
    io::Buffer<byte> data;
};


class ProcessManager:
    private io::FileCollectionObserver {

public:
    /**
     * A singleton class. Return the instance;
     */
    static ProcessManager& instance();

    ProcessProxy* getProcessByName(const char* name);

    void registerProcess(Process* p);

    ~ProcessManager();
private:
    ProcessManager();

    void discover();

    void observe( int fd, int events );

    std::map<std::string, size_t> m_proc_db; 
    std::vector<Process*> m_proc_table;

    std::map<std::string, ProcessAddress> m_extern_proc_db;

    io::DatagramSocket m_outside;
    io::FileCollection m_filecollection;

    logger::LogContext& m_log;
    os::Mutex m_mutex;
    os::Thread* m_filecol_thread;
    io::UnixAddress m_addr;
};

}

template <>
inline int putObject( io::Putter& p, const proc::ManagerPacket& pkt ) {
    p.putInt32be(pkt.origin_pid);
    p.putByte(pkt.type);
    putObject(p, pkt.data);
    return 0;
}

template <>
inline int getObject( io::Getter& g, proc::ManagerPacket& pkt ) {
    g.getInt32be(pkt.origin_pid);
    g.getByte((byte&)pkt.type);
    getObject( g, pkt.data );
    return 0;
}

#endif /* PROCESSMANAGER_HPP_ */
