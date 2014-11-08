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

INTERFACE BaseIO {
public:
    virtual ssize_t read( byte* out, size_t len ) = 0;
    virtual ssize_t write( const byte* in, size_t len ) = 0;
    inline virtual ~BaseIO(){}
};

}

#endif /* BASEIO_HPP_ */
