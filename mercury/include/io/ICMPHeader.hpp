#ifndef ICMPHEADER_HPP_
#define ICMPHEADER_HPP_

/*
 * Author: jrahm
 * created: 2015/01/22
 * ICMPHeader.hpp: <description>
 */

#include <Prelude>
#include <io/binary/Putter.hpp>
#include <io/binary/Getter.hpp>

#define ICMPHEADER_SIZE 8

namespace io {


namespace icmp {
    enum Type {
           ECHOREPLY=0	/* Echo Reply */
         , DEST_UNREACH=3	/* Destination Unreachable */
         , SOURCE_QUENCH=4	/* Source Quench */
         , REDIRECT=5	/* Redirect (change route) */
         , ECHO=8	/* Echo Request */
         , TIME_EXCEEDED=11	/* Time Exceeded */
         , PARAMETERPROB=12	/* Parameter Problem */
         , TIMESTAMP=13	/* Timestamp Request */
         , TIMESTAMPREPLY=14	/* Timestamp Reply */
         , INFO_REQUEST=15	/* Information Request */
         , INFO_REPLY=16	/* Information Reply */
         , ADDRESS=17	/* Address Mask Request */
         , ADDRESSREPLY=18	/* Address Mask Reply */
    };

	enum Code {
          NET_UNREACH=0	/* Network Unreachable */
        , HOST_UNREACH=1	/* Host Unreachable */
        , PROT_UNREACH=2	/* Protocol Unreachable */
        , PORT_UNREACH=3	/* Port Unreachable */
        , FRAG_NEEDED=4	/* Fragmentation Needed/DF set */
        , SR_FAILED=5	/* Source Route failed */
        , NET_UNKNOWN=6
        , HOST_UNKNOWN=7
        , HOST_ISOLATED=8
        , NET_ANO=9
        , HOST_ANO=10
        , NET_UNR_TOS=11
        , HOST_UNR_TOS=12
        , PKT_FILTERED=13	/* Packet filtered */
        , PREC_VIOLATION=14	/* Precedence violation */
        , PREC_CUTOFF=15	/* Precedence cut off */
	};
}

class ICMPHeader {
public:
	static ICMPHeader Echo(u16_t echoid, u16_t echo_seq);

	inline void setType(icmp::Type type) { this->type = type; }
	inline void setCode(icmp::Code code) { this->code = code; }
	inline void setEchoId(u16_t echo_id) { this->echo_id = echo_id; }
	inline void setEchoSequence(u16_t echo_seq) { this->echo_seq = echo_seq; }
	inline void setGateway(u32_t gateway) { this->gw = gateway; }
	inline void setMTU(u16_t mtu) { this->mtu = mtu; }
	inline void setChecksum(u16_t check) { this->checksum = check; }

	inline icmp::Type getType() { return type; }
	inline icmp::Code getCode() { return code; }
	inline u16_t getEchoId() { return echo_id; }
	inline u16_t getEchoSequence() { return echo_seq; }
	inline u32_t getGateway() { return gw; }
	inline u16_t getMTU() { return mtu; }
	inline u16_t getChecksum() { return checksum; }

	size_t getChecksumSum() ;
	inline ICMPHeader() {}
private:
	inline ICMPHeader(icmp::Type t, icmp::Code c, u32_t g):
		type(t), code(c), checksum(0), gw(g) {}

	icmp::Type type;
	icmp::Code code;
	u16_t checksum;
	union {
		struct {
        	u16_t echo_id; 
    		u16_t echo_seq;
		};
		u32_t gw;
		struct {
			u16_t unused;
			u16_t mtu;
		};
	};
};

};

inline int putObject( io::Putter& putter, io::ICMPHeader& hdr ) {
	putter.putByte( hdr.getType() );
	putter.putByte( hdr.getCode() );
	putter.putInt16be( hdr.getChecksum() );
    printf("Gateway: %04x\n", hdr.getGateway());
	putter.putInt16be( hdr.getEchoId() );
	putter.putInt16be( hdr.getEchoSequence() );
	return 0;
}

inline int getObject( io::Getter& getter, io::ICMPHeader& hdr ) {
	byte b; u32_t gw;

	b = getter.getByte();
	hdr.setType((io::icmp::Type)b);
	b = getter.getByte();
	hdr.setCode((io::icmp::Code)b);
	getter.getInt16be();
	gw = getter.getInt32be();
	hdr.setGateway(gw);

	return 0;
}

#endif /* ICMPHEADER_HPP_ */
