#ifndef PRELUDE_HPP_
#define PRELUDE_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * prelude.hpp: <description>
 */

#include <types.h>

#include <cstdlib>
#include <string>
#include <cstring>

/**
 * @brief A simple class that acts like a pointer, but deletes itself after going out of scope
 *
 * This is very similar to the unique pointer definition from C++ 11,
 * but the support for C++ 11 on these different platforms is shotty at best
 */
template <class T>
class uptr { /* unique pointer definition from C++ 11 */
public:
    uptr(T*& val) {
        mine = val;
        val = NULL;
    }

    uptr() {
        mine = NULL;
    }

    T& operator *() {
        return *mine;
    };

    T* operator ->() {
        return get();
    }

    T*& get() {
        return mine;
    }

    T*& cleanref() {
        delete mine;
        mine = NULL;
        return mine;
    }

    ~uptr() {
        delete mine;
    }
private:
    T* mine;
};

class Exception {
public:
    inline Exception( const std::string& message ) : message(message) {}
    inline Exception( const char* message ) : message(message) {}
    inline Exception() : message() {};

    const char* getMessage() const {
        return message.c_str();
    }
protected:
    void setMessage( const char* mesg ) {
        this->message = mesg;
    }
    std::string message;
};

class CException : public Exception {
public:
    inline CException(const char* message, int rc) :
        Exception(message) {
        this->rc = rc;
        this->message += ": ";
        this->message += strerror(rc);
    }

    inline CException( int rc ) {
        this->rc = rc;
        setMessage(strerror(this->rc));
    }

    inline int getCode() {
        return rc;
    }
private:
    int rc;
};

#endif /* PRELUDE_HPP_ */
