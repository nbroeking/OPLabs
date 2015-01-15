#ifndef BASEIO_HPP_
#define BASEIO_HPP_

/*
 * Author: jrahm
 * created: 2014/11/07
 * BaseIO.hpp: <description>
 */

#include <types.h>

#include <cstdio>

namespace io {

/**
 * @brief A simple class that can read, write and close.
 *
 * Basic I/O dascriptor. Can read, write and close.
 */
class BaseIO {
public:
    /**
     * @brief read data into a byte array
     * @param out the buffer to read into
     * @param len the lengthe of the buffer
     *
     * This function should read at most <code>len</code>
     * bytes from the input source.
     *
     * @return the nuber of bytes read
     */
    virtual ssize_t read( byte* out, size_t len ) = 0;

    /**
     * @brief write data to this I/O descriptor.
     *
     * @param in the data to write
     * @param len the length of the data
     *
     * Write len bytes to the stream
     *
     * @return the number of bytes written
     */
    virtual ssize_t write( const byte* in, size_t len ) = 0;

    /**
     * @brief close this I/O stream
     */
    virtual int close() = 0;


    inline virtual ~BaseIO(){}
};

}

#endif /* BASEIO_HPP_ */
