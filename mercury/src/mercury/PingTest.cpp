#include <mercury/PingTest.hpp>
#include <io/binary/BufferGetter.hpp>
#include <io/binary/BufferPutter.hpp>

using namespace logger;
using namespace proc;
using namespace io;

PingTest::PingTest():
    Process("PingTest"),
    m_state_machine(*this, IDLE),
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
    
    switch(pckt.type) {
        case BEGIN_TEST:
            m_state_machine.sendStimulus(START_TEST);
            break;
    }
}

void PingTest::observe(int fd, int events) {
    (void)fd; (void)events;
    io::ICMPPacket packet;
    uptr<SocketAddress> addr;
    
    m_sock->receive(packet, addr.get());
    m_recieve_queue.push(packet);

    m_state_machine.sendStimulus(PING_RECIEVED);
}

PingState PingTest::startTest() {
    m_sock = new ICMPSocket();
    m_sock->init();
    m_sock->setNonBlocking();
    getFileCollection().subscribe(FileCollection::SUBSCRIBE_READ, m_sock, this) ;

    u16_t echo_id = 0x5555;

    ICMPPacket pkt;
}

const char* ping_packet_type_names[] = {
    "BeginTest", "Ack", "Results"
};

std::string toString( const PingTestPacketType& typ ) {
    if ( typ >= NTYPES ) return "(Unknown)";
    return ping_packet_type_names[typ];
}
