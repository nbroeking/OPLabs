
#include <io/Socket.hpp>
#include <io/ServerSocket.hpp>
#include <io/StringWriter.hpp>

#include <cstdio>

using namespace std ;
using namespace io ;

int main( int argc, char** argv ) {
    int rc;
    /* Simple C++ socket program. Connect to 5432 on localhost
     * and print Hello, World */

    StreamSocket sock;
    rc = sock.connect( "localhost", 5432 );

    if( rc != 0 ) {
        perror("Failed to connect socket");
        return 1;
    }

    StringWriter writer;

    writer.setBaseIO( &sock );
    writer.printf("Hello, World from %s\n", "Josh");

    return 0;
}
