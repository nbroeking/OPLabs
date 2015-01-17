#include <io/ICMPSocket.hpp>
#include <test.h>
#include <resolv.h>
#include <netdb.h>

#include <unistd.h>
#include <string>
#include <cstring>

using namespace io;
using namespace std;

const char* host;

int runping( ICMPSocket& sock ) {
    sock.setTimeout(500 MILLIS);

    string data( "Hello, World!" );
    byte mesg[128 - sizeof(icmphdr)];

    fill(mesg, mesg + sizeof(mesg), 0);
    copy(data.c_str(), data.c_str() + data.size(), mesg);

    size_t count = 0;
    struct hostent *hname;
    struct sockaddr_in addr;

    struct sockaddr_in r_addr;
    socklen_t r_len = sizeof(sockaddr_in);

    LOG("Attempting to ping %s\n", host);
    hname = gethostbyname(host);
    TEST_BOOL( "getHostByName", hname != NULL ) ;

    addr.sin_family = hname->h_addrtype;
    addr.sin_port = 0 ;
    addr.sin_addr.s_addr = *(long*)hname->h_addr;

    ICMPPacket pckt;
    icmphdr hdr;

    hdr.type = ICMP_ECHO;
    hdr.un.echo.id = getpid();
    hdr.un.echo.sequence = count ++;

    pckt.setHeader(hdr);
    pckt.setMessage(mesg, sizeof(mesg));

    ICMPPacket pkt;

    TEST_BOOL( "SocketSend", sock.send(pckt, (sockaddr*)&addr, sizeof(sockaddr_in)) > 0 );
    TEST_BOOL( "SocketReceive", sock.receive(pkt, (struct sockaddr*)&r_addr, r_len) == 0 );

    TEST_EQ_INT( "PingMessage", strcmp((const char*)pkt.getMessage(), "Hello, World!"), 0 );

    sock.setTimeout(1 MICROS);
    TEST_BOOL( "TestTimeout", sock.receive(pkt, (struct sockaddr*)&r_addr, r_len) != 0 );

    return 0;
}

int main(int argc, char** argv) {
    ICMPSocket sock;

    if ( geteuid() != 0 ) {
        printf("Need to be run as root\n");
        return 0;
    }

    if ( argc > 1 ) {
        host = argv[1];
    } else {
        host = "8.8.8.8";
    }

    TEST_BOOL( "ICMPSocket::init", sock.init() >= 0 );
    TEST_EQ_INT( "setNonBlocking", sock.setNonBlocking(), 0 );
    TEST_EQ_INT( "setBlocking", sock.setNonBlocking(false), 0 );

    TEST_FN( runping( sock ) );
}
