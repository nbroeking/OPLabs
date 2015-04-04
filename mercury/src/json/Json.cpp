#include <json/Json.hpp>

#include <jansson.h>

namespace json {

Json Json::operator[](const std::string& str) const {
    json_type typ = getType();

    if(typ == JSON_OBJECT) {
        return json_object_get(m_raw, str.c_str());
    }

    throw JsonException("Json not an object in operator[]");
}

Json Json::operator[](size_t idx) const {
    json_type typ = getType();
    if(typ == JSON_ARRAY) {
        return json_array_get(m_raw, idx);
    }
    
    throw JsonException("Json not a list in operator[]");
}

size_t Json::arraySize() const {
    json_type typ = getType();
    if(typ == JSON_ARRAY) {
        return json_array_size(m_raw);
    }

    throw JsonException("Json not a list in arraySize()");
}

json_type Json::getType() const {
    return json_typeof(m_raw);
}

Json* Json::parse(const std::string& str) {
    json_error_t err;
    json_t* root = json_loads(str.c_str(), str.size(), &err);

    if(!root) {
        throw JsonException("Failed to parse json string");
    }
    Json* ret = new Json(root);
    ret->m_free = true;
    return ret;
}

bool Json::operator==(int i) const {
    json_type typ = getType();

    if(typ == JSON_REAL) {
        return i == json_real_value(m_raw);
    }

    if(typ != JSON_INTEGER) {
        return false;
    }
    return i == json_integer_value(m_raw);
}

bool Json::operator==(const std::string& str) const {
    json_type typ = getType();
    if(typ != JSON_STRING) {
        return false;
    }
    return str == std::string(json_string_value(m_raw));
}

std::string Json::stringValue() const {
    json_type typ = getType();
    if(typ != JSON_STRING)
        throw JsonException("Type not string in stringValue");
    return std::string(json_string_value(m_raw));
}

s64_t Json::intValue() const {
    if(getType() == JSON_REAL) {
        return (s64_t) json_real_value(m_raw);
    }
    if(getType() != JSON_INTEGER) {
        throw JsonException("Type not real or float in intValue()");
    }
    return json_integer_value(m_raw);
}

Json::Json() {
    m_raw = json_object();
}

Json::Json(const std::string& str) {
    m_raw = json_string(str.c_str());
    m_free = true;
}

Json::Json(s64_t i) {
    m_raw = json_integer(i);
    m_free = true;
}

Json::Json(const std::vector<Json>& vec) {
    m_raw = json_array();
    m_free = true;

    std::vector<Json>::const_iterator itr;
    FOR_EACH(itr, vec) {
        push_back(*itr);
    }
}

void Json::push_back(const Json& jsn) {
    if(getType() != JSON_ARRAY) {
        throw JsonException("Not an array in push_back");
    }
    json_array_append_new(m_raw, jsn.m_raw);
    jsn.m_free = false;
}

void Json::setAttribute(const char* attr, const Json& json) {
    if(getType() != JSON_OBJECT) {
        throw JsonException("Not an object in setAttribute");
    }
    json_object_set_new(m_raw, attr, json.m_raw);
    json.m_free = false;
}


}
