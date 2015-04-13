#ifndef LOGSERVER_HPP_
#define LOGSERVER_HPP_

/*
 * Author: jrahm
 * created: 2015/04/10
 * LogServer.hpp: <description>
 */

#include <os/Runnable.hpp>

#include <io/SocketAddress.hpp>
#include <io/FileCollection.hpp>
#include <io/RingBuffer.hpp>

namespace logger {

class LogServer: public os::ManagedRunnable {
public:
    LogServer(const io::SocketAddress& bind_addr);

    void run();
    void stop();
private:

    void addConnection(io::StreamSocket* sock);
    void removeConnection(io::StreamSocket* sock);

    void handleCmdLine(io::BaseIO* init, const char* cmdline);

    class Observer;
    friend class Observer;

    Observer* m_observer;
    io::FileCollection m_file_collection;
    io::StreamServerSocket m_sock;
};

}

#endif /* LOGSERVER_HPP_ */
