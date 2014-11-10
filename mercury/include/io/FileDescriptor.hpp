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

    inline ssize_t read( byte* out, size_t len ) OVERRIDE {
        return ::read( m_fd, out, len );
    }

    inline ssize_t write( const byte* in, size_t len ) OVERRIDE {
        return ::write( m_fd, in, len );
    }

    inline int close() OVERRIDE { return ::close( m_fd ); }

private:
    int m_fd;
};

}

#endif /* FILEDESCRIPTOR_HPP_ */
