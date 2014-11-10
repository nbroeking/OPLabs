#include <io/binary/StreamPutter.hpp>

namespace io {

StreamPutter::StreamPutter( BaseIO*& base_io, bool take, size_t buflen ) {
    m_buffer = new byte[buflen];
    m_is_owner = take;
    m_buffer_ptr = 0;
    m_buffer_total_size = buflen;

    if( m_is_owner ) base_io = NULL;
}

void StreamPutter::flush( ) {
    if( m_buffer_ptr > 0 ) {
	    m_base_io->write( m_buffer, m_buffer_ptr );
    }

    m_buffer_ptr = 0;
}

int StreamPutter::putByte( byte b ) {
    m_buffer[m_buffer_ptr++] = b;

    if( m_buffer_ptr == m_buffer_total_size ) {
        flush();
    }

    return 0;
}

int StreamPutter::close() {
    flush();
    return m_base_io->close();
}

StreamPutter::~StreamPutter() {
    if( m_is_owner ) {
        close();
        delete m_base_io;
    } else {
        flush();
    }

    delete[] m_buffer;
}
    
}
