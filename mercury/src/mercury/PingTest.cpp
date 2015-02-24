#include <mercury/PingTest.hpp>
#include <io/binary/BufferGetter.hpp>
#include <io/binary/BufferPutter.hpp>

using namespace logger;
using namespace proc;
using namespace io;

PingTest::PingTest():
    Process("PingTest"),
    m_log( LogManager::instance().getLogContext("Process", "PingTest") ) {
}

void PingTest::messageReceivedCallback( 
    proc::ProcessAddressProxy& from_addr,
    const byte* bytes, size_t size ) {

    PingTestPacket packet;
    BufferGetter getter(bytes, size);
    
    try {
        getObject( getter, packet );
        this->sendAck( from_addr );
        this->onPacketReceived( packet );
    } catch ( Exception& e ) {
        m_log.printfln(ERROR, "Error deserializing packet: %s", e.getMessage() );
    }
}

void PingTest::sendAck( ProcessProxy& proxy ) {
    PingTestPacket ack;
    ack.type = ACK;
    byte buf[128];
    BufferPutter putter(buf, 128);
    putObject(putter, ack);
    proxy.sendMessage(this->getAddress(), buf, putter.getSize());
}

void PingTest::onPacketReceived(const PingTestPacket& pckt) {
    m_log.printfln(DEBUG, "Received packet of type %s", toString(pckt.type).c_str());
}

const char* ping_packet_type_names[] = {
    "BeginTest", "Ack", "Results"
};

std::string toString( const PingTestPacketType& typ ) {
    if ( typ >= NTYPES ) return "(Unknown)";
    return ping_packet_type_names[typ];
}
