#ifndef JSONMERCURYTEMPL_HPP_
#define JSONMERCURYTEMPL_HPP_

/*
 * Author: jrahm
 * created: 2015/04/04
 * JsonMercuryTempl.hpp: <description>
 */

#include <json/Json.hpp>

#include <io/Inet4Address.hpp>

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


#endif /* JSONMERCURYTEMPL_HPP_ */
