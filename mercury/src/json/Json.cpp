#include <json/Json.hpp>

#include <jansson.h>

namespace json {

Json Json::operator[](const std::string& str) {
    json_type typ = getType();

    if(typ == JSON_OBJECT) {
        return json_object_get(m_raw, str.c_str());
    }

    throw JsonException("Json not an object in operator[]");
}

Json Json::operator[](size_t idx) {
    json_type typ = getType();
    if(typ == JSON_ARRAY) {
        return json_array_get(m_raw, idx);
    }
    
    throw JsonException("Json not a list in operator[]");
}

size_t Json::arraySize() {
    json_type typ = getType();
    if(typ == JSON_ARRAY) {
        return json_array_size(m_raw);
    }

    throw JsonException("Json not a list in arraySize()");
}

json_type Json::getType() {
    return json_typeof(m_raw);
}

Json* Json::parse(const std::string& str) {
    json_error_t err;
    json_t* root = json_loads(str.c_str(), str.size(), &err);
    Json* ret = new Json(root);
    ret->m_free = true;
    return ret;
}

}
