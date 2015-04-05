#include <mercury/dns/Test.hpp>

namespace dns {

TestConfig TestConfig::parse(json_t* json) {
    if(!json_is_object(json)) {
        throw Exception("Test config not a json object");
    }

    json_t* cursor;
    

}

}
