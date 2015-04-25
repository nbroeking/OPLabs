#ifndef RINGBUFFER_HPP_
#define RINGBUFFER_HPP_

/*
 * Author: jrahm
 * created: 2015/04/11
 * RingBuffer.hpp: <description>
 */

#include <io/BaseIO.hpp>

namespace io {

class RingBuffer: public BaseIO {
public:
    RingBuffer(size_t size);
    RingBuffer(const RingBuffer& oth);

    ssize_t read(byte* into, size_t size);
    ssize_t write(const byte* out, size_t size);
    ssize_t peek(byte* into, size_t size);
    int close();
    void setSize(size_t buffer_size);
    inline size_t getSize() {
        return m_size;
    }

    void operator=(const RingBuffer& oth);

    ~RingBuffer();

private:
    byte* m_buffer;
    size_t m_size;
    size_t m_startptr;
    size_t m_endptr;
};

}

#endif /* RINGBUFFER_HPP_ */
