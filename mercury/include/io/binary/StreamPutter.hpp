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

class StreamPutter : public Putter {
public:
    StreamPutter( BaseIO*& base_io, bool take=false, size_t buflen=1024 );

    virtual int putByte( byte b ) OVERRIDE ;

    virtual void flush();

    virtual int close();

    virtual ~StreamPutter();

private:
    byte* m_buffer;
    size_t m_buffer_ptr;
    size_t m_buffer_total_size;

    BaseIO* m_base_io;
    bool m_is_owner;
};

}

#endif /* STREAMPUTTER_HPP_ */
