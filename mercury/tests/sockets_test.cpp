#include <io/Socket.hpp>
#include <io/ServerSocket.hpp>
#include <io/StringWriter.hpp>
#include <test.h>

#include <unistd.h>

using namespace io;
using namespace std;

int server() {
    StreamServerSocket* server_sock = StreamServerSocket::createAndBindListen(5432);

    TEST_BOOL("createBindAndListen", server_sock != NULL);
    LOG("Server socket opened\n");

    StreamSocket* sock = server_sock->accept();
    

    StringWriter writer;

    writer.stealBaseIO( (BaseIO*&)sock );
    writer.printf("Test%d", 0);

    delete sock;
    return 0;
}

int client() {
    StreamSocket* sock = new StreamSocket();
    int rc = sock->connect("localhost", 5432);
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
    if( fork() == 0) {
        server();
    } else {
        sleep(1);
        return client();
    }
}
