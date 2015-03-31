#include <Prelude>
#include <io/SocketAddress.hpp>
#include <io/Inet4Address.hpp>

#include <vector>
#include <jansson.h>

using namespace std;
using namespace io;

namespace mercury {

struct TestSuiteConfiguration {
    vector< uptr<SocketAddress> > ookla_ips;
    vector< uptr<SocketAddress> > ping_ips;
    vector< uptr<SocketAddress> > dns_ips;
};

class JsonParseException: public Exception {
public:
    JsonParseException(const char* message): Exception(message){}
};

string test_suite_log(const TestSuiteConfiguration& conf) {
    string ret;
    vector< uptr<SocketAddress> >::const_iterator itr;

    ret += "ookla_ips: [";
    FOR_EACH(itr, conf.ookla_ips) {
        ret += (*itr)->toString() + ", ";
    }
    ret += "] ping_ips: [";
    FOR_EACH(itr, conf.ping_ips) {
        ret += (*itr)->toString() + ", ";
    }
    ret += "] dns_ips: [";
    FOR_EACH(itr, conf.dns_ips) {
        ret += (*itr)->toString() + ", ";
    }
    ret += "]";

    return ret;
}
/* returns true if the status is ok */
bool _parseTestSuiteConfiguration(json_t*& root, json_t*& config, json_t*& cur, const char* data, TestSuiteConfiguration& conf) {
    json_error_t err;

    root = json_loads(data, 0, &err);

    if(!root) {
        throw JsonParseException("Unable to parse root node");
    }
    
    if(!json_is_object(root)) {
        throw JsonParseException("Root node is not an object");
    }

    cur = json_object_get(root, "status");
    if(!json_is_string(cur) || strcmp(json_string_value(cur), "failure") == 0) {
        return false;
    }
    config = json_object_get(root, "config");

    static const char* json_ips_arr[] = {
        "ookla_ips", "ping_ips", "dns_ips", NULL
    };

    vector< uptr<SocketAddress> >* vectors[] = {
        &conf.ookla_ips, &conf.ping_ips, &conf.dns_ips
    };

    size_t i;
    for(i = 0; json_ips_arr[i]; ++ i) {
        cur = json_object_get(config, json_ips_arr[i]);
        if(!json_is_array(cur)) {
            throw JsonParseException("Ips node is not an array");
        }
        for(size_t j = 0; j < json_array_size(cur); ++ j) {
            json_t* jsn = json_array_get(cur, j);
            if(!json_is_string(jsn)) {
                throw JsonParseException("Element inside ip vector not a string");
            }
            SocketAddress* addr = new Inet4Address(json_string_value(jsn), 0);
            vectors[i]->push_back(addr);
        }
    }

    return true;
}

bool parseTestSuiteConfiguration(const char* data, TestSuiteConfiguration& conf) {
    json_t* root = NULL;
    json_t* config = NULL;
    json_t* cur = NULL;

    bool ret;
    try {
        ret = _parseTestSuiteConfiguration(root, config, cur, data, conf);
        json_delete(root);
        return ret;
    } catch(Exception& ex) {
        json_delete(root);
        throw ex;
    }
}

}
