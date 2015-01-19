#include <io/Socket.hpp>
#include <io/ServerSocket.hpp>
#include <io/StringWriter.hpp>
#include <test.h>

#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <io/Inet4Address.hpp>
#include <io/Inet6Address.hpp>
#include <io/UnixAddress.hpp>

#include <cstdlib>

using namespace io;
using namespace std;

int server(const SocketAddress& bind_addr) {

    try {
        StreamServerSocket* server_sock =
            StreamServerSocket::createAndBindListen(bind_addr);
    
        TEST_BOOL("createBindAndListen", server_sock != NULL);
        LOG("%s","Server socket opened\n");
    
        StreamSocket* sock = server_sock->accept();
        StringWriter writer;
    
        LOG("%s","Connection accepted\n");
        byte *b = (byte*)"Test0";
        sock->write(b, sizeof("Test0"));

        return 0;
    } catch (CException& err) {
        LOG("Error running server: %s\n", err.getMessage());
        return 1;
    }
}

int client(const SocketAddress& connect) {
    StreamSocket* sock = new StreamSocket();

    int rc = 1;

    try {
        rc = sock->connect( connect );
    } catch (InetParseException& p) {
        LOG("Caught exception: %s\n", p.getMessage());
    }

    TEST_EQ_INT( "connect", rc, 0 );

    char test[4096];
    std::fill(test, test+4096, 0);
    int size = sock->read( (byte*)test, 4096 - 1);
    printf("size = %d\n", size);
    TEST_BOOL( "read", size >= 0 );
    test[size]=0;
    TEST_EQ( "strncmp", strncmp(test, "Test0", sizeof("Test0")), 0 ) ;

    delete sock;
    return 0;
}

int test_socket( const SocketAddress& addr ) {
    int rc;

    if( fork() == 0 ) {
        int rc = server(addr);
        if ( rc ) {
            LOG("Server Exit with bad code %s", "");
        }
        exit(rc);
    } else {
        sleep(1);
        TEST_FN( client(addr) );
    }

    wait( & rc );
    TEST_EQ( "ServerExit", rc, 0 );

    return 0;
}

#define LOCALHOST 0x7f000001
int main( int argc, char** argv ) {
    (void) argc;
    (void) argv;
    int rc = 0;

    srand(time(NULL));
    uint16_t port = rand() % 50000 + 5000 ;
    uint16_t port6 = rand() % 50000 + 5000 ;

    Inet4Address iaddr(LOCALHOST, port);
    UnixAddress uaddr("/tmp/myunixsocket.sock");
    Inet6Address i6addr = Inet6Address::fromString("::1", port6);

    uaddr.unlink();

    LOG("Testing IPv4%s", "");
    TEST_FN( test_socket(iaddr) );
    LOG("Testing IPv6%s", "");
    TEST_FN( test_socket(i6addr) );
    LOG("Testing Unix%s", "");
    TEST_FN( test_socket(uaddr) );

    uaddr.unlink();

    return rc;
}
