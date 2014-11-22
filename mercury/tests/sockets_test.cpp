#include <io/Socket.hpp>
#include <io/ServerSocket.hpp>
#include <io/StringWriter.hpp>
#include <test.h>

#include <unistd.h>
#include <sys/wait.h>

#include <cstdlib>

using namespace io;
using namespace std;

int server(uint16_t port) {
    StreamServerSocket* server_sock = StreamServerSocket::createAndBindListen(port);

    TEST_BOOL("createBindAndListen", server_sock != NULL);
    LOG("%s","Server socket opened\n");

    StreamSocket* sock = server_sock->accept();
    

    StringWriter writer;

    writer.stealBaseIO( (BaseIO*&)sock );
    writer.printf("Test%d", 0);

    return 0;
}

int client(uint16_t port) {
    StreamSocket* sock = new StreamSocket();
    int rc = sock->connect("localhost", port);
    TEST_EQ_INT( "connect", rc, 0 );

    char test[4096];
    int size = sock->read( (byte*)test, 4096 - 1);
    TEST_BOOL( "read", size > 0 );
    test[size]=0;
    TEST_EQ( "strncmp", strncmp(test, "Test0", size), 0 ) ;

    delete sock;
    return 0;
}

int main( int argc, char** argv ) {
    (void) argc;
    (void) argv;
    int rc;

    uint16_t port = rand() % 50000 + 5000 ;

    if( fork() == 0) {
        server(port);
    } else {
        sleep(1);
        TEST_FN( client(port) );
    }

    wait( & rc );
    TEST_EQ( "ServerExit", rc, 0 );

    return rc;
}
