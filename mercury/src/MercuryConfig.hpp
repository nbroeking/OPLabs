#ifndef MERCURYCONFIG_HPP_
#define MERCURYCONFIG_HPP_

#include <cstdio>

#include <io/FilePointer.hpp>
#include <io/Inet4Address.hpp>
#include <io/Resolv.hpp>
#include <io/ServerSocket.hpp>
#include <io/Socket.hpp>
#include <io/StringWriter.hpp>
#include <io/binary/StreamGetter.hpp>

#include <json/Json.hpp>
#include <json/JsonMercuryTempl.hpp>

#include <log/LogManager.hpp>
#include <log/LogServer.hpp>

#include <mercury/Mercury.hpp>
#include <mercury/monitor/Monitor.hpp>

#include <os/Condition.hpp>

#include <sys/wait.h>

#include <errno.h>
#include <signal.h>
#include <unistd.h>

using namespace logger ;
using namespace mercury ;
using namespace std ;
using namespace io ;
using namespace os ;
using namespace json ;

/*
 * Author: jrahm
 * created: 2015/04/17
 * MercuryConfig.hpp: <description>
 */

class MercuryConfig {
public:
    MercuryConfig():
        logEverything(false),
        controller_url("http://127.0.0.1:5000/"),
        bind_ip(new Inet4Address("127.0.0.1", 8639)),
        log_out(NULL),
        colors(true),
        default_level(NULL),
        log_server(NULL)
        {}

    bool logEverything;
    std::string controller_url;
    SocketAddress* bind_ip;
    BaseIO* log_out;
    bool colors;
    LogLevel* default_level;
    
    SocketAddress* log_server;
    timeout_t monitor_interval;
    u32_t monitor_datapoints;
};

template<>
struct JsonBasicConvert<MercuryConfig> {
    static MercuryConfig convert(const json::Json& jsn) {
        MercuryConfig ret;
        ret.logEverything = jsn.hasAttribute("logEverything") && jsn["logEverything"] != 0;
        SocketAddress* old;

        if(jsn.hasAttribute("controller_url")) {
            ret.controller_url = jsn["controller_url"].stringValue();
        }
        if(jsn.hasAttribute("bind_ip")) {
            old = ret.bind_ip;
            ret.bind_ip = jsn["bind_ip"].convert<SocketAddress*>();
            delete old;
        }

        if(jsn.hasAttribute("logFile")) {
            FILE* f = fopen(jsn["logFile"].stringValue().c_str(), "w");
            if(f) {
                io::FilePointer* fp = new io::FilePointer(f);
                ret.log_out = fp;
            } else {
                fprintf(stderr, "Unable to open log file. Logging to stdout\n");
            }
        }

        if(jsn.hasAttribute("defaultLogLevel")) {
            ret.default_level = LogLevel::getLogLevelByName(jsn["defaultLogLevel"].stringValue().c_str());
        }

        if(jsn.hasAttribute("logColors")) {
            ret.colors = jsn["logColors"] != 0;
        }

        if(jsn.hasAttribute("logServerBindAddress")) {
            ret.log_server = jsn["logServerBindAddress"].convert<SocketAddress*>();
            if(ret.log_server->linkProtocol() == AF_UNIX) {
                dynamic_cast<UnixAddress*>(ret.log_server)->unlink();
            }
        }

        if(jsn.hasAttribute("monitorInterval")) {
            ret.monitor_interval = jsn["monitorInterval"].convert<u64_t>() MILLIS;
        } else {
            ret.monitor_interval = 60 SECS;
        }

        if(jsn.hasAttribute("monitorNumberOfDatapoints")) {
            ret.monitor_datapoints = jsn["monitorNumberOfDatapoints"].intValue() MILLIS;
        } else {
            ret.monitor_datapoints = 1024;
        }

        return ret;
    }
};

#endif /* MERCURYCONFIG_HPP_ */
