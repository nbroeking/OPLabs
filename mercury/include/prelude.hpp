#ifndef PRELUDE_HPP_
#define PRELUDE_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * prelude.hpp: <description>
 */

#include <types.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>

#define ENUM_TO_STRING(EnumType, Num, ...) \
    inline std::string toString(EnumType en) { \
        const char* names[] = { __VA_ARGS__ }; \
        return en >= Num ? "(Unknown)" : names[en]; \
    }

#define FOR_EACH(itr, vec)\
    for( itr = vec.begin() ; itr != vec.end() ; ++ itr )

/**
 * @brief A simple class that acts like a pointer, but deletes itself after going out of scope
 *
 * This is very similar to the unique pointer definition from C++ 11,
 * but the support for C++ 11 on these different platforms is shotty at best
 */
template <class T>
class uptr { /* unique pointer definition from C++ 11 */
public:
    template <class U>
    uptr(U* val) {
        mine = val;
        nref = new int(1);
    }


    uptr() {
        mine = NULL;
        nref = NULL;
    }

    uptr(const uptr<T>& oth) {
        mine = oth.mine;
        nref = oth.nref;
        if(nref) (*nref) ++;
    }

    void operator=( const uptr<T>& oth ) {
        unref();
        mine = oth.mine;
        nref = oth.nref;
        if(nref) (*nref) ++;
    }

    bool operator==(T* oth) {
        return mine == oth;
    }

    void unref() {
        if(nref) {
            (*nref) --;
            if( *nref == 0 ) {
                delete mine;
                delete nref;
            }
        }
    }

    void operator=( T* oth ) {
        unref();
        nref = new int(1);
        mine = oth;
    }

    T& operator *() {
        return *mine;
    };

    T* operator ->() {
        return get();
    }

    const T* operator ->() const {
        return mine;
    }

    T*& get() {
        return mine;
    }

    const T*& get() const {
        return mine;
    }

    T*& cleanref() {
        delete mine;
        mine = NULL;
        return mine;
    }

    /* Free the pointer from this structure */
    T* save() {
        T* ret = mine;
        mine = NULL;
        return ret;
    }

    ~uptr() {
        unref();
    }
private:
    T* mine;
    int* nref;
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

class InvalidArgumentException: public Exception {
public:
    inline InvalidArgumentException( const char* msg ):
        Exception(msg) {}
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
