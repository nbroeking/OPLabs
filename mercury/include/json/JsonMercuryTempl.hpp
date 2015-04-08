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

template <>
struct JsonBasicConvert<io::Inet4Address> {
    static io::Inet4Address convert(const json::Json& jsn) {
        std::string str = jsn.stringValue();
        size_t itr = str.find(':');
        if(itr == std::string::npos) {
            return io::Inet4Address(str.c_str(), 0);
        } else {
            std::string addr = str.substr(0, itr);
            std::string port = str.substr(itr + 1);
            return io::Inet4Address(addr.c_str(), atoi(port.c_str()));
        }
    }

    static json::Json convert_from(const io::Inet4Address& addr) {
        return json::Json(addr.toString());
    }
};

template <>
struct JsonBasicConvert<io::Inet6Address> {
    static io::Inet6Address convert(const json::Json& jsn) {
        std::string str = jsn.stringValue();
        io::Inet6Address ret;
        
        if(str.size() == 0) { throw json::JsonException("Expected string of length at least 1"); }

        if(str[0] == '[') { // [x:y:z::x]:port
            size_t idx = str.find(']');
            if(idx == std::string::npos) {
                throw json::JsonException("Bad format of ipv6 string. No ']' after [");
            }

            std::string sub = str.substr(1, idx);
            idx += 1;

            u16_t port = 0;

            if(idx != str.length()) {
                if(str[idx] != ':') {
                    throw json::JsonException("Bad format of ipv6 string. No ':' after []");
                }

                port = atoi(str.substr(idx+1).c_str());
            }

            ret = io::Inet6Address::fromString(sub.c_str(), port);
        }

        ret = io::Inet6Address::fromString(str.c_str(), 0);
        return ret;
    }

    static json::Json convert_from(const io::Inet6Address& addr) {
        return json::Json(addr.toString());
    }
};

template <>
struct JsonBasicConvert<io::SocketAddress*> {
    static io::SocketAddress* convert(const json::Json& jsn) {
        std::string str = jsn.stringValue();
        size_t idx;
        if((idx = str.find(':')) != std::string::npos &&
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
