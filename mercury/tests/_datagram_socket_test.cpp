#include <io/DatagramSocket.hpp>
#include <log/LogManager.hpp>

#include <io/Inet4Address.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>
#include <cstring>

using namespace logger;
using namespace io;
using namespace std;

int server( int port ) {
    LogContext& log = LogManager::instance().getLogContext("Tests", "DatagramSocket");  
    log.printfln(INFO, "Starting the server.");
    
    DatagramSocket sock;
    Inet4Address addr(INADDR_ANY, port);

    try {
        sock.bind(addr);
        byte bytes[1024];
    
        uptr<SocketAddress> from;
        ssize_t l = sock.receive(bytes, sizeof(bytes), &from.cleanref() );

        log.printfln(DEBUG, "%d bytes received from %s", l, from->toString().c_str());
        log.printHex(DEBUG, bytes, l);
    
        const char* send = "It is a nice day today";
        sock.sendTo((const byte*)send, strlen(send), *from);
    } catch ( DatagramSocketException& e ) {
        log.printfln(ERROR, "%s", e.getMessage());
    }

    sock.close();
    return 0;
}

int main( int argc, char** argv ) {
    (void) argc; (void) argv;
    LogContext& log = LogManager::instance().getLogContext("Tests", "DatagramSocket");  
    log.setEnabled( true );
    log.printfln(INFO, "Datagram Test Socket");
    srand( time(NULL) );

    DatagramSocket sock;
    Inet4Address addr("0.0.0.0", 3824);

    int port = rand() % 1024 + 5000;
    Inet4Address server_addr("127.0.0.1", port);

    if( fork() == 0 ) {
        return server(port);
    }

    sleep(1);

    byte bytes[1024];
    strncpy((char*)bytes, "Hello, World", sizeof(bytes));
    bytes[sizeof(bytes)-1] = 0;

    uptr<SocketAddress> from;
    try {
        sock.bind(addr);
        sock.sendTo( bytes, strlen("Hello, World"), server_addr );
        ssize_t nread = sock.receive(bytes, sizeof(bytes), &from.cleanref());
        log.printfln(DEBUG, "Received bytes");
        log.printHex(DEBUG, bytes, nread);
    } catch(DatagramSocketException& e) {
        log.printfln(ERROR, "%s", e.getMessage());
    }

    int rc;
    wait(&rc);

    return 0;
}
