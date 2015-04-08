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
    Json(const std::string& stnr);
    Json(s64_t i);
    Json(const std::vector<Json>& vec);
    
    template <class T>
    static Json from(const T& thing) {
        return JsonBasicConvert<T>::convert_from(thing);
    }

    Json operator[](const std::string& str) const;   
    Json operator[](size_t s) const;

    size_t arraySize() const;
    std::string stringValue() const;
    s64_t intValue() const;

    json_type getType() const;

    bool operator==(int i) const;
    bool operator==(const std::string& str) const;

    void push_back(const Json& json); /* for the templates */
    inline void append(const Json& json) { push_back(json); };
    void setAttribute(const char* attr, const Json& json);

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

    ~Json() {
        if(m_free) json_delete(m_raw);
    };

    inline std::string toString() {
        std::string ret;
        char* tmp = json_dumps(m_raw, 0);
        ret = tmp;
        delete[] tmp;
        return ret;
    }

    static Json* parse(const std::string& str);

private:
    Json(json_t* raw):
        m_raw(raw), m_free(false){}
    
    json_t* m_raw;
    mutable bool m_free; /* Used for bookeepnig */
};

}

template <>
struct JsonBasicConvert<s64_t> {
    static s64_t convert(const json::Json& jsn) {
        return jsn.intValue();
    }

    static json::Json convert_from(const s64_t& i) {
        return json::Json(i);
    }
};

template <>
struct JsonBasicConvert<std::string> {
    static std::string convert(const json::Json& jsn) {
        return jsn.stringValue();
    }

    static json::Json convert_from(const std::string& str) {
        return json::Json(str);
    }
};


#endif /* INCLUDE_JSON_JSON_ */
