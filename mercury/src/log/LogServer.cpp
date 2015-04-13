#include <log/LogServer.hpp>

#include <signal.h>
#include <sstream>

#include <io/FilePointer.hpp>
#include <io/FileDescriptor.hpp>

namespace logger {

using namespace io;
using namespace std;

void hpipe(int v) {
    (void) v;
}

class LogServer::Observer: public FileCollectionObserver {
public:
    Observer(LogServer& outer): m_outer(outer) {}

    void observe(HasRawFd* raw_fd, int events) {
        (void)events;
        if(raw_fd == &m_outer.m_sock) {
            StreamSocket* client;
            client = m_outer.m_sock.accept();

            m_outer.addConnection(client);
        } else {
            byte cmdline[4096];
            BaseIO* client;
            client = dynamic_cast<BaseIO*>(raw_fd);
            ssize_t l = client->read(cmdline, 4095);
            cmdline[l] = 0;
            m_outer.handleCmdLine(client, (char*)cmdline);
        }
    }

    void onUnsubscribe(HasRawFd* fd) {
        m_outer.removeConnection(dynamic_cast<StreamSocket*>(fd));
    }

    LogServer& m_outer; 
};

LogServer::LogServer(const io::SocketAddress& addr)
    {
    m_sock.bind(addr);
    m_sock.listen(5);

    m_observer = new Observer(*this);
    m_file_collection.subscribe(
        FileCollection::SERVER_SOCKET,
        &m_sock, m_observer);

    m_file_collection.subscribe(FileCollection::SUBSCRIBE_READ,
        new FileDescriptor(0),
        m_observer);
}

void LogServer::addConnection(StreamSocket* sock) {
    LogManager::instance().addLogOutput(sock, true);
    m_file_collection.subscribe(FileCollection::SUBSCRIBE_READ, sock, m_observer);
}

void LogServer::removeConnection(StreamSocket* sock) {
    LogManager::instance().removeLogOutput(sock);
    delete sock;
}

void LogServer::run() {
    m_file_collection.run();
}

void LogServer::stop() {
    m_file_collection.stop();
}

LogLevel ECHO(30, "Echo", 20, true);
LogLevel ECHO_W(33, "Echo", 20, true);

void print_help(LogContext& log) {
    log.printfln(ECHO, "Sparky - Your Log Interface");
    log.printfln(ECHO, "    echo          [STRING] -- echo a message");
    log.printfln(ECHO, "    set_level     [LEVEL]  -- set the log level");
    log.printfln(ECHO, "    disable_group [GROUP]  -- disable a log group");
    log.printfln(ECHO, "    enable_group  [GROUP]  -- enable a log group");
    log.printfln(ECHO, "    help                   -- print this help");
}
void LogServer::handleCmdLine(io::BaseIO* init, const char* cmdline) {
    string str(cmdline);
    stringstream stream(str);

    string word;
    stream >> word;
     
    LogManager& manager = LogManager::instance();
    LogContext& log = manager.getLogContext("LS", "Sparky");
    log.setEnabled(true);
    log.setMinimumLevel(TRACE);

    if(word == "echo") {
        getline(stream, word);
        log.printfln(ECHO, "@%p: %s", init, word.c_str());
    } else if(word == "set_level") {
        stream >> word;
        LogLevel* ll = LogLevel::getLogLevelByName(word.c_str());
        if(!ll) {
            log.printfln(ECHO_W, "@%p: %s is not a log level", init, word.c_str());
        } else {
            manager.setLogLevelForAll(*ll);
            log.printfln(ECHO, "@%p: Min log level set to %s", init, word.c_str());
        }
    } else if (word == "disable_group") {
        stream >> word;
        manager.enableAllForMajor(word, false);
        log.printfln(ECHO, "@%p All logs in group %s are now disabled",
                            init, word.c_str());
    } else if (word == "enable_group" ) {
        stream >> word;
        log.printfln(ECHO, "@%p All logs in group %s are now enables",
                            init, word.c_str());
        manager.enableAllForMajor(word, true);
    } else if (word == "help") {
        print_help(log);
    }
    
    else {
        log.printfln(ECHO_W, "@%p: Unknown command", init, word.c_str());
    }
}



}
