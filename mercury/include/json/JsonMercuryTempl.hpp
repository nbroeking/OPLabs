#ifndef JSONMERCURYTEMPL_HPP_
#define JSONMERCURYTEMPL_HPP_

/*
 * Author: jrahm
 * created: 2015/04/04
 * JsonMercuryTempl.hpp: <description>
 */

#include <json/Json.hpp>

#include <io/Inet4Address.hpp>
#include <io/Inet6Address.hpp>
#include <io/UnixAddress.hpp>

template <>
struct JsonBasicConvert<io::Inet4Address> {
    static io::Inet4Address convert(const json::Json& jsn) {
        return io::Inet4Address::fromString(jsn.stringValue().c_str());
    }

    static json::Json convert_from(const io::Inet4Address& addr) {
        return json::Json::fromString(addr.toString().c_str());
    }
};

template <>
struct JsonBasicConvert<io::Inet6Address> {
    static io::Inet6Address convert(const json::Json& jsn) {
        return io::Inet6Address::fromString(jsn.stringValue().c_str());
    }

    static json::Json convert_from(const io::Inet6Address& addr) {
        return json::Json::fromString(addr.toString().c_str());
    }
};

template <>
struct JsonBasicConvert<io::SocketAddress*> {
    static io::SocketAddress* convert(const json::Json& jsn) {
        std::string str = jsn.stringValue();
        size_t idx;
        if(str.compare(0, 6, "unix:/") == 0) {
            if(str.size() < 7 || str[6] != '/') {
                throw json::JsonException("Parse error in UnixAddress");
            }
            return new io::UnixAddress(str.substr(6).c_str());
        } else if((idx = str.find(':')) != std::string::npos &&
            (str.rfind(':') != idx)) { /* If there are 2 ':' in the address */
            io::Inet6Address* ret = new io::Inet6Address(
                JsonBasicConvert<io::Inet6Address>::convert(jsn)
            );
            return ret;
        } else {
            io::Inet4Address* ret = new io::Inet4Address(
                JsonBasicConvert<io::Inet4Address>::convert(jsn)   
            );
            return ret;
        }

        throw new json::JsonException("Not a socket address");
    }
};

#endif /* JSONMERCURYTEMPL_HPP_ */
