#include <io/RingBuffer.hpp>

namespace io {

RingBuffer::RingBuffer(size_t size):
    m_buffer(new byte[size]),
    m_size(size),
    m_startptr(0),
    m_endptr(1) {}

RingBuffer::RingBuffer(const RingBuffer& oth) {
    m_buffer = NULL;
    *this = oth;
}

ssize_t RingBuffer::write(const byte* in, size_t size) {
    size_t s = size;
    bool hit_start = false;
    while(size) {
        m_buffer[m_endptr] = *in;
        m_endptr = (m_endptr + 1) % m_size;

        if(m_endptr == m_startptr) {
            m_startptr = (m_startptr + 1) % m_size;
        }
        -- size;
        ++ in;
    }

    if(hit_start) {
        m_startptr = m_endptr + 1;
    }

    return s;
}

int RingBuffer::close() {
    return 0;
}

void RingBuffer::setSize(size_t size) {
    byte* oldb = m_buffer;
    m_buffer = new byte[size];
    std::copy(oldb, oldb + m_size, m_buffer);
    m_size = size;
    delete[] oldb;
}

void RingBuffer::operator=(const RingBuffer& oth) {
    delete m_buffer;
    m_buffer = new byte[oth.m_size];
    std::copy(oth.m_buffer, oth.m_buffer + oth.m_size, m_buffer);
    m_size = oth.m_size;
    m_startptr = oth.m_startptr;
    m_endptr = oth.m_endptr;
}

ssize_t RingBuffer::read(byte* into, size_t size) {
    #define inc(a) a = (a + 1) % m_size;
    size_t bytes_read = peek(into, size);
    m_startptr = (m_startptr + bytes_read) % m_size;
    return bytes_read;
}

ssize_t RingBuffer::peek(byte* into, size_t size) {
    size_t bytes_read = 0;
    size_t cur = m_startptr;
    while(cur != m_endptr && size) {
        into[bytes_read++] = m_buffer[cur];
        cur = (cur + 1) % m_size;
        size --;
    }
    return bytes_read;
}

RingBuffer::~RingBuffer() {
    delete[] m_buffer;
}

}
