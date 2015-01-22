#include <log/LogManager.hpp>
#include <io/FileCollection.hpp>
#include <io/DatagramSocket.hpp>
#include <io/Inet4Address.hpp>
#include <io/UnixAddress.hpp>
#include <sys/poll.h>
#include <unistd.h>

using namespace logger;
using namespace std;
using namespace io;

class PrintSocketObserver : public SingleFileCollectionObserver {
public:

DatagramSocket* sock;

virtual void observe( int fd, int events ) {
    (void) fd;
    LogContext& log = LogManager::instance().getLogContext("Tests", "FileCollection");
    log.printfln(DEBUG, "observed called");

    if( (events & POLLIN) != 0 ) {
        log.printfln(DEBUG, "Can read from socket");

        try {
            uptr<SocketAddress> addr;
            byte bytes[1024];
            ssize_t len = sock->receive(bytes, 1024, addr.cleanref());
            
            log.printHex(DEBUG, bytes, len);
        } catch ( Exception& e ) {
            log.printfln(ERROR, "Error caught: %s", e.getMessage());
        }
    }
}

virtual HasRawFd* getFd() {
    return sock;
}


};

int client() {
    sleep(1);
    try {
        UnixAddress addr("/tmp/mynix.sock.cli");
        addr.unlink();
        UnixAddress saddr("/tmp/mynix.sock");
        DatagramSocket sock;
        sock.bind(addr);
    
        byte data[1024];
        sock.sendTo(data, 1024, saddr);
    } catch ( DatagramSocketException& e ) {
        printf("Error %s", e.getMessage());
    }

    return 0;
}

int main( int argc, char** argv ) {
    (void) argc ; (void) argv ;
    LogContext& log = LogManager::instance().getLogContext("Tests", "FileCollection");
    log.setEnabled(true);
    LogManager::instance().enableAllForMajor("IO");

    log.printfln(INFO, "Start file collection test");

    if( fork() == 0 ) {
        return client();
    }

    FileCollection collection;
    // Inet4Address addr("0.0.0.0", 5432);
    UnixAddress addr("/tmp/mynix.sock");
    addr.unlink();

    PrintSocketObserver* observer = new PrintSocketObserver();
    DatagramSocket sock;
    sock.bind(addr);
    observer->sock = &sock;

    collection.subscribeForRead(observer);
    collection.run();
}
