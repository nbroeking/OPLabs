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

    /**
     * @brief Return the a process by its given name
     * @param name The name of the process
     * @return The Process given that name, or NULL if no process by that name is found
     */
    ProcessProxy* getProcessByName(const char* name);

    /**
     * @brief return a new Process by the address of that process
     * @param addr The address of the process
     * @return a new ProcessProxy representing that process
     */
    ProcessProxy* getProcessByAddress(const ProcessAddress& addr);

    /**
     * @brief Register a process with this manager.
     * @param p The process to assign.
     * @return The new address of the process.
     */
    ProcessAddress registerProcess(Process* p);

    ~ProcessManager();
private:
    ProcessManager();

    void discover();

    void observe( int fd, int events );

    std::map<std::string, size_t> m_proc_db; 
    std::vector<Process*> m_proc_table;

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
    pkt.origin_pid = g.getInt32be();
    pkt.type = (proc::PacketType)g.getByte();
    getObject( g, pkt.data );
    return 0;
}

#endif /* PROCESSMANAGER_HPP_ */
