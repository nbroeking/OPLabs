#ifndef BUFFERPUTTER_HPP_
#define BUFFERPUTTER_HPP_

/*
 * Author: jrahm
 * created: 2015/01/22
 * BufferPutter.hpp: <description>
 */

#include <io/binary/Putter.hpp>
#include <Prelude>

namespace io {

class OutOfBufferSpaceException: public Exception {
public:
	OutOfBufferSpaceException( const char* msg ):
		Exception(msg) {}
};

/**
 * @brief A putter that puts bytes to a byte array.
 */
class BufferPutter : public Putter {
public:
	inline BufferPutter(byte* bytes, size_t len):
		bytes(bytes), len(len), cur(0) {}

	virtual inline int putByte(byte b) OVERRIDE {
		if( cur == len ) {
			char buf[1024];
			snprintf(buf, sizeof(buf), "Overflow at index %lu. %lu byte exceeded\n",
				(unsigned long)this->cur,
				(unsigned long)this->len);
			throw OutOfBufferSpaceException(buf);
		}
		bytes[cur ++] = b;
	}
private:
	byte* bytes;
	size_t len;
	size_t cur;
};

}

#endif /* BUFFERPUTTER_HPP_ */
