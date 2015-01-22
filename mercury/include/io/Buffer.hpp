#ifndef BUFFER_HPP_
#define BUFFER_HPP_

/*
 * Author: jrahm
 * created: 2015/01/20
 * Buffer.hpp: <description>
 */

#include <io/binary/Putter.hpp>
#include <io/binary/Getter.hpp>
#include <prelude.hpp>
#include <algorithm>
#include <cstring>

namespace io {

template <class T>
class Buffer {
public:
    Buffer(const Buffer<T>& oth) {
        array = NULL;
        * this = oth;
    }

    Buffer() {
        array = NULL;
        this->len = 0;
    }

    Buffer(size_t len) {
        array = new T[this->len = len];
    }

    Buffer(const T* ptr, size_t len) {
        array = new T[this->len = len];
        std::copy(ptr, ptr + len, array);
    }

    const Buffer<T>& operator=(const Buffer<T>& oth) {
        if( &oth == this ) return * this;

        delete array;
        array = new T[len = oth.length()];
        std::copy(oth.array, oth.array + oth.length(), array);
        return * this;
    }

    size_t length() const { return len; }

    T& operator[](size_t i) {
        return array[i];
    }

    const T& operator[](size_t i) const {
        return array[i];
    }

    T* rawPointer() {
        return array;
    }

    const T* rawPointer() const {
        return array;
    }

    ~Buffer() { delete[] array; };

private:
    size_t len;
    T* array;
};

}

inline int putObject( io::Putter& putter, const io::Buffer<byte>& buf ) {
    return putter.putInt32be(buf.length()) ||
           putIterator(putter, buf.rawPointer(), buf.rawPointer() + buf.length());
}

inline int getObject( io::Getter& getter, io::Buffer<byte>& buf ) {
    uint32_t size ;
    getter.getInt32be( size );

    io::Buffer<byte> n(size);
    for ( size_t i = 0 ; i < size ; ++ i ) {
        getObject(getter, n[i]);
    }
    buf = n;

    return 0;
}

#endif /* BUFFER_HPP_ */
