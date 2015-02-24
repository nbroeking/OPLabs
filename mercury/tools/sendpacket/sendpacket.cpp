#include <io/Socket.hpp>
#include <io/Inet4Address.hpp>

#include <log/LogManager.hpp>

using namespace io;
using namespace logger;

byte mercury_magic_cookie[ 32 ] = {
    0xe2, 0x1a, 0x14, 0xc8, 0xa2, 0x35, 0x0a, 0x92,
    0xaf, 0x1a, 0xff, 0xd6, 0x35, 0x2b, 0xa4, 0xf3, 
    0x97, 0x79, 0xaf, 0xb5, 0xc1, 0x23, 0x43, 0xf0,
    0xf7, 0x14, 0x17, 0x62, 0x53, 0x4a, 0xa9, 0x7e, 
};

int main( int argc, char** argv ) {
	LogContext& m_log = LogManager::instance().getLogContext("SendPacket", "SendPacket");
	LogManager::instance().logEverything();
	m_log.printfln(INFO, "This is a thing");

	try {
    	StreamSocket sock;
    	Inet4Address addr("127.0.0.1", 8639);
    	sock.connect(addr);
    
    	srand(time(NULL));
    
    	union {
    		struct {
    			byte cookie[32];
    			byte id[32];
    		}_str;
    		byte bytes[64];
    	};
    
    	std::copy(mercury_magic_cookie, mercury_magic_cookie + 32, _str.cookie);
    	int i = 0;
    	for( ; i < 32 ; ++ i ) {
    		_str.id[i] = rand() % 256;
    	}
    
    
    	sock.write(bytes, sizeof(bytes));
    	sock.close();
    
    	return 0;
	} catch ( Exception& exp ) {
		m_log.printfln(FATAL, "Uncaught exception %s", exp.getMessage());
		return 1;
	}
}
