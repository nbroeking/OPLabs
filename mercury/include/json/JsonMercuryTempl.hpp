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
        return json::Json::fromString(addr.toString().c_str());
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

            std::string sub = str.substr(1, idx-1);
            idx += 1;

            u16_t port = 0;

            if(idx != str.length()) {
                if(str[idx] != ':') {
                    throw json::JsonException("Bad format of ipv6 string. No ':' after []");
                }

                std::string portstr = str.substr(idx+1, -1);
                port = atoi(portstr.c_str());
            }

            ret = io::Inet6Address::fromString(sub.c_str(), port);
        } else {
            ret = io::Inet6Address::fromString(str.c_str(), 0);
        }
        return ret;
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
