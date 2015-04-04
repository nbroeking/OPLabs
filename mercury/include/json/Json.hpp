#ifndef INCLUDE_JSON_JSON_
#define INCLUDE_JSON_JSON_

/*
 * Author: jrahm
 * created: 2015/04/03
 * Json.hpp: <description>
 */

#include <jansson.h>
#include <Prelude>

namespace json {

class JsonException: public Exception {
public:
    JsonException(const char* err): Exception(err){}
};

class Json {
public:
    Json operator[](const std::string& str);   
    Json operator[](size_t s);

    size_t arraySize();
    std::string stringValue();

    json_type getType();

    ~Json() {
        if(m_free) json_delete(m_raw);
    };

    static Json* parse(const std::string& str);

private:
    Json(json_t* raw):
        m_raw(raw), m_free(false){}
    
    json_t* m_raw;
    bool m_free;
};

}

#endif /* INCLUDE_JSON_JSON_ */
