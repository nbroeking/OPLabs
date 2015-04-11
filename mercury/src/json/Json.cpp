#include <json/Json.hpp>

#include <jansson.h>
#include <io/FileNotFoundException.hpp>

namespace json {

Json Json::operator[](const std::string& str) const {
    json_type typ = getType();

    if(typ == JSON_OBJECT) {
        json_t* ret = json_object_get(m_raw, str.c_str());
        if(!ret) throw JsonException((std::string("No such attribute ") + str.c_str()).c_str());
        return Json( json_copy(ret) );
    }

    throw JsonException("Json not an object in operator[]");
}

bool Json::hasAttribute(const char* attr) const {
    if(getType() != JSON_OBJECT) return false;
    return json_object_get(m_raw, attr) != NULL;
}

Json Json::operator[](size_t idx) const {
    json_type typ = getType();
    if(typ == JSON_ARRAY) {
        return Json( json_copy(json_array_get(m_raw, idx)) );
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
    return ret;
}

Json* Json::fromFile(const char* filename) {
    json_error_t err;
    FILE* f = fopen(filename, "r");
    if(!f) {
        char buf[1024];
        snprintf(buf, sizeof(buf), "Unable to open file: %s", filename);
        throw io::FileNotFoundException(buf);
    }

    json_t* root = json_loadf(f, 0, &err);

    if(!root) {
        throw JsonException("Failed to parse json string");
    }

    Json* ret = new Json(root);
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

bool Json::operator==(f64_t i) const {
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

Json Json::fromString(const char* str) {
    json_t* raw = json_string(str);
    return Json(raw);
}

Json Json::fromInt(s64_t f) {
    json_t* raw = json_integer(f);
    return Json(raw);
}

Json Json::fromFloat(f64_t f) {
    json_t* raw = json_real(f);
    return Json(raw);
}

Json Json::fromVector(const std::vector<Json>& vec) {
    json_t* raw = json_array();
    Json ret(raw);

    std::vector<Json>::const_iterator itr;
    FOR_EACH(itr, vec) {
        ret.push_back(*itr);
    }

    return ret;
}

f64_t Json::floatValue() const {
    if(getType() == JSON_REAL) {
        return (f64_t) json_real_value(m_raw);
    }
    if(getType() != JSON_INTEGER) {
        throw JsonException("Type not real or float in intValue()");
    }
    return (f64_t)json_integer_value(m_raw);
}

void Json::push_back(const Json& jsn) {
    if(getType() != JSON_ARRAY) {
        throw JsonException("Not an array in push_back");
    }
    json_array_append_new(m_raw, jsn.m_raw);
}

void Json::setAttribute(const char* attr, const Json& json) {
    if(getType() != JSON_OBJECT) {
        throw JsonException("Not an object in setAttribute");
    }
    json_object_set_new(m_raw, attr, json_copy(json.m_raw));
}


}
