#ifndef FILEDESCRIPTOR_HPP_
#define FILEDESCRIPTOR_HPP_

/*
 * Author: jrahm
 * created: 2014/11/09
 * FileDescriptor.hpp: <description>
 */

#include <io/BaseIO.hpp>

#include <unistd.h>

namespace io {

class FileDescriptor : public BaseIO {
public:
    inline FileDescriptor( int fd ):
        m_fd( fd ) {}

    ssize_t read( byte* out, size_t len ) OVERRIDE ;

    ssize_t write( const byte* in, size_t len ) OVERRIDE ;

    int close() OVERRIDE ;

private:
    int m_fd;
};

}

#endif /* FILEDESCRIPTOR_HPP_ */
