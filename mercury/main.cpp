
#include <io/Socket.hpp>
#include <io/ServerSocket.hpp>
#include <io/StringWriter.hpp>

#include <io/Inet4Address.hpp>

#include <io/binary/StreamGetter.hpp>

#include <cstdio>

using namespace std ;
using namespace io ;

int main( int argc, char** argv ) {
    (void) argc;
    (void) argv;
    int rc;
    /* Simple C++ socket program. Connect to 5432 on localhost
     * and print Hello, World */

    StreamSocket sock;
    try {
        Inet4Address connect = Inet4Address::fromString("127.0.0.1", 5432);
        rc = sock.connect( connect );
    } catch (InetParseException& p) {
    }

    if( rc != 0 ) {
        perror("Failed to connect socket");
        return 1;
    }

    StringWriter writer;

    writer.setBaseIO( &sock );
    writer.printf("Hello, World from %s\n", "Josh");

	uint32_t i;
	StreamGetter getter( &sock );
	rc = getter.getInt32le(i) ;

	if( rc ) {
		printf("Getter failed! rc=%d\n", rc);
	} else {
		printf("Read integer 0x%08x\n", i);
	}

	std::string name;
	rc = getObject( getter, name );

	if( rc ) {
		printf("Failed to get string\n");
	} else {
		printf("name: %s\n", name.c_str());
	}

    return 0;
}
