#ifndef DNSPACKET_HPP_
#define DNSPACKET_HPP_

/*
 * Author: jrahm
 * created: 2015/04/12
 * DnsPacket.hpp: <description>
 */

#include <Prelude>

#include <io/binary/Putter.hpp>

namespace io {

class DnsPacket {
public:
    inline DnsPacket(const char* host, u16_t pid):
        m_hostname(host), m_packet_id(pid) {}

    inline DnsPacket():
        m_hostname(), m_packet_id(0) {}

    void setHostname(const char* host);
    inline const char* getHostname() const{ return m_hostname.c_str(); };

    void setPacketId(u16_t pid) ;
    inline u16_t getPacketId() const { return m_packet_id; };

    void putterSerialize(io::Putter& out) const ;
private:
    std::string m_hostname;
    u16_t m_packet_id;
};

}

inline void putObject(io::Putter& putter, const io::DnsPacket& packet) {
    packet.putterSerialize(putter);
}

#endif /* DNSPACKET_HPP_ */
