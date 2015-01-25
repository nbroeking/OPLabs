#ifndef BUFFERGETTER_HPP_
#define BUFFERGETTER_HPP_

/*
 * Author: jrahm
 * created: 2015/01/25
 * BufferGetter.hpp: <description>
 */

#include <Prelude>
#include <io/binary/Getter.hpp>

namespace io {

class BufferGetter: public Getter {
public:
	inline BufferGetter( const byte* blob, size_t blob_len ) :
		m_blob(blob), m_blob_len( blob_len ), m_cursor(0) {}

	virtual inline byte getByte() {
		if ( m_cursor >= m_blob_len ) {
            char buf[1024];
            snprintf(buf, sizeof(buf), "Gettor underflow at position %u", (unsigned int)m_cursor);
            throw GetterUnderflowException(buf);
		}
		return m_blob[m_cursor ++];
	}

private:
	const byte* m_blob;
	size_t m_blob_len;
	size_t m_cursor;
};

}

#endif /* BUFFERGETTER_HPP_ */
