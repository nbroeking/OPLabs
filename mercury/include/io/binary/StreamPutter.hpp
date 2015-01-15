#ifndef STREAMPUTTER_HPP_
#define STREAMPUTTER_HPP_

/*
 * Author: jrahm
 * created: 2014/11/09
 * StreamPutter.hpp: <description>
 */

#include <io/binary/Putter.hpp>
#include <io/BaseIO.hpp>

namespace io {

/**
 * @brief A Putter for use with an IO Stream
 */
class StreamPutter : public Putter {
public:
    /**
     *  - 
     * @brief An instance of Putter for use on streams
     * 
     * @param base_io The BaseIO to delegate to
     * @param take Should this object take control of the BaseIO?
     * @param buflen The size of the internal buffer
     *
     * This class is buffered, this means that it will write at most
     * <code>buflen</code> before flushing the stream
     */
    StreamPutter( BaseIO*& base_io, bool take=false, size_t buflen=1024 );
    StreamPutter( BaseIO*, size_t buflen=1024 );

    virtual int putByte( byte b ) OVERRIDE ;

    /**
     * @brief Flush the underlying output stream
     */
    virtual void flush();

    /**
     * @brief close the underlying output stream
     * @return 0 on success or an error code
     */
    virtual int close();

    virtual ~StreamPutter();

private:
    /* The array of bytes for the buffer */
    byte* m_buffer;
    /* The pointer to the current byte in the buffer */
    size_t m_buffer_ptr;
    /* The total size of the buffer */
    size_t m_buffer_total_size;

    BaseIO* m_base_io;
    bool m_is_owner;
};

}

#endif /* STREAMPUTTER_HPP_ */
