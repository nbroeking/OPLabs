#include <io/FileDescriptor.hpp>
namespace io {

ssize_t FileDescriptor::read( byte* out, size_t len ) {
    return ::read( m_fd, out, len );
}

ssize_t FileDescriptor::write( const byte* in, size_t len ) {
    return ::write( m_fd, in, len );
}

int FileDescriptor::close() { return ::close( m_fd ); }

}
