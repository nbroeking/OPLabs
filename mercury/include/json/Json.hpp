#ifndef INCLUDE_JSON_JSON_
#define INCLUDE_JSON_JSON_

/*
 * Author: jrahm
 * created: 2015/04/03
 * Json.hpp: <description>
 */

#include <jansson.h>
#include <vector>
#include <Prelude>

namespace json {
    class Json;
}

template <class T>
struct JsonBasicConvert {
    static T convert(const json::Json& jsn);
    static json::Json convert_from(const T& t);
};

namespace json {

class JsonException: public Exception {
public:
    JsonException(const char* err): Exception(err){}
};


class Json {
public:
    Json();
    Json(const Json& jsn) {
        m_raw = json_copy(jsn.m_raw);
    }
    // Json(const std::string& stnr);
    // Json(s64_t i);
    // Json(f64_t f);
    // Json(const std::vector<Json>& vec);
    
    static Json fromInt(s64_t f);
    static Json fromFloat(f64_t f);
    static Json fromString(const char* str);
    static Json fromVector(const std::vector<Json>& vec);
    
    template <class T>
    static Json from(const T& thing) {
        return JsonBasicConvert<T>::convert_from(thing);
    }

    Json operator[](const std::string& str) const;   
    Json operator[](size_t s) const;

    size_t arraySize() const;
    std::string stringValue() const;

    s64_t intValue() const;
    f64_t floatValue() const;

    json_type getType() const;

    bool operator==(int i) const;
    bool operator==(f64_t i) const;
    bool operator==(const std::string& str) const;

    inline bool operator!=(int i) const {
        return (!this->operator==(i));
    }

    inline bool operator!=(const std::string& i) const {
        return (!this->operator==(i));
    }
    void push_back(const Json& json); /* for the templates */
    inline void append(const Json& json) { push_back(json); };
    void setAttribute(const char* attr, const Json& json);
    bool hasAttribute(const char* attr) const;

    template <class T>
    T convert() const {
        return JsonBasicConvert<T>::convert(*this);
    }

    template <class T>
    void toVector(std::vector<T>& vec,
                  T(*convert)(const Json&) =
                    JsonBasicConvert<T>::convert) {
        size_t size = arraySize();
        for(size_t i = 0; i < size; ++ i) {
            vec.push_back(convert((*this)[i]));
        }
    }

    Json& operator=(const Json& oth) {
        m_raw = json_copy(oth.m_raw);
        return *this;
    }

    ~Json() {
        json_delete(m_raw);
    };

    inline std::string toString() {
        std::string ret;
        char* tmp = json_dumps(m_raw, 0);
        ret = tmp;
        delete[] tmp;
        return ret;
    }

    static Json* parse(const std::string& str);
    static Json* fromFile(const char* filename);

private:
    Json(json_t* raw):
        m_raw(raw){}
    
    json_t* m_raw;
};

}

template <>
struct JsonBasicConvert<s64_t> {
    static s64_t convert(const json::Json& jsn) {
        return jsn.intValue();
    }

    static json::Json convert_from(const s64_t& i) {
        return json::Json::fromInt(i);
    }
};

template <>
struct JsonBasicConvert<std::string> {
    static std::string convert(const json::Json& jsn) {
        return jsn.stringValue();
    }

    static json::Json convert_from(const std::string& str) {
        json::Json ret = json::Json::fromString(str.c_str());
        return ret;
    }
};


#endif /* INCLUDE_JSON_JSON_ */
