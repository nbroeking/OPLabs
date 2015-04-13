#ifndef FILEDESCRIPTOR_HPP_
#define FILEDESCRIPTOR_HPP_

/*
 * Author: jrahm
 * created: 2014/11/09
 * FileDescriptor.hpp: <description>
 */

#include <io/BaseIO.hpp>
#include <io/HasRawFd.hpp>

#include <unistd.h>

namespace io {

/**
 * @brief A very simple class that wraps a C style file descriptor
 */
class FileDescriptor : public BaseIO, public HasRawFd {
public:
    /** 
     * @brief Wrap a C style file descriptor
     * @param fd the file descriptor to wrap
     */
    inline FileDescriptor( int fd ):
        m_fd( fd ) {}

    /**
     *  @see BaseIO::read(byte*,size_t)
     */
    ssize_t read( byte* out, size_t len ) OVERRIDE ;

    /**
     *  @see BaseIO::write(const byte*,size_t)
     */
    ssize_t write( const byte* in, size_t len ) OVERRIDE ;

    /**
     *  @see BaseIO::close()
     */
    int close() OVERRIDE ;

    inline int getRawFd() { return m_fd; }

private:
    int m_fd;
};

}

#endif /* FILEDESCRIPTOR_HPP_ */
