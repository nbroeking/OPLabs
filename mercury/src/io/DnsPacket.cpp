#include <io/DnsPacket.hpp>

#include <missing.h>

using namespace io;
using namespace std;

namespace io {

void DnsPacket::setHostname(const char* host) {
    m_hostname = host;
}

void DnsPacket::setPacketId(u16_t packet) {
    m_packet_id = packet;
}

void DnsPacket::putterSerialize(io::Putter& putter) const {
    vector<string> splitted;
    split(m_hostname, '.', splitted);

    putter.putByte((m_packet_id >> 8) & 0xFF);
    putter.putByte(m_packet_id & 0xFF);

    putter.putByte(0x01);
    putter.putByte(0x00);
    putter.putByte(0x00);
    putter.putByte(0x01);

    for(size_t i = 0; i < 6; ++ i) {
        putter.putByte(0x00);
    }

    vector<string>::iterator itr;
    FOR_EACH(itr, splitted) {
        putter.putByte(itr->size());
        putIterator(putter, itr->begin(), itr->end());
    }

    putter.putByte(0x00);
    putter.putByte(0x00);
    putter.putByte(0x01);
    putter.putByte(0x00);
    putter.putByte(0x01);
}

};
