#include <io/ICMPHeader.hpp>
#include <io/binary/BufferPutter.hpp>

namespace io {

ICMPHeader ICMPHeader::Echo(u16_t echoid, u16_t echo_seq) {
	ICMPHeader ret(icmp::ECHO, (icmp::Code)0, 0);
	ret.echo_id = echoid;
	ret.echo_seq = echo_seq;
	return ret;
}

size_t ICMPHeader::getChecksumSum() {
    return ((type << 8) + code) + 0 + echo_id + echo_seq;
}

}
