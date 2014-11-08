#ifndef GETTER_HPP_
#define GETTER_HPP_

/*
 * Author: jrahm
 * created: 2014/11/08
 * Getter.hpp: <description>
 */

#include <types.h>

#include <vector>
#include <cstdlib>

#include <string>
#include <algorithm>

namespace io {

/* General class use very nicely for the decoding and
 * encoding of binary data */
class Getter {
public:
	Getter( byte*& blob, size_t blob_len ) :
		m_blob(blob), m_blob_len( blob_len ), m_cursor(0) {
			blob = NULL;
		}

	/* Get a byte from the input source */
	virtual inline int getByte( byte& b ) {
		if ( m_cursor >= m_blob_len ) {
			return 1;
		}
		b = m_blob[m_cursor ++];
		return 0;
	}
	
	virtual inline int getInt16le( uint16_t& i ) {
		byte high, low;
		int rc = getByte( low ) || getByte( high ) ;
		i = high << 8 | low ;
		return rc;
	}

	virtual inline int getInt16be( uint16_t& i ){
		byte high, low;
		int rc = getByte( high ) || getByte( low ) ;
		i = high << 8 | low ;
		return rc;
	}

	virtual inline int getInt32le( uint32_t& i ) {
		uint16_t high, low;
		int rc = getInt16le( low ) || getInt16le( high ) ;
		i = high << 16 | low ;
		return rc;
	}

	virtual inline int getInt32be( uint32_t& i ) {
		uint16_t high, low;
		int rc = getInt16be( high ) || getInt16be( low ) ;
		i = high << 16 | low ;
		return rc;
	}

	virtual inline int getInt64le( uint64_t& i ) {
		uint32_t high, low;
		int rc = getInt32le( low ) || getInt32le( high ) ;
		i = ((uint64_t)high) << 32 | low ;
		return rc;
	}

	virtual inline int getInt64be( uint64_t& i ) {
		uint32_t high, low;
		int rc = getInt32be( high ) || getInt32be( low ) ;
		i = ((uint64_t)high) << 32 | low ;
		return rc;
	}

	virtual inline int getBytes( byte* into, size_t len ) {
		if( m_blob_len - m_cursor <= len ) return 1;
		std::copy( m_blob + m_cursor, m_blob + m_cursor + len, into );
		m_cursor += len;
		return 0;
	}

	virtual inline ~Getter(){}

protected:
	Getter() : m_blob(NULL), m_blob_len(0), m_cursor(0) {}

	byte* m_blob ;
	size_t m_blob_len ;
	size_t m_cursor ;
};

}

template <class T>
inline int getObject( io::Getter& g, T& t );

template <>
inline int getObject( io::Getter& g, std::string& str ) {
	uint32_t len;

	if( g.getInt32le(len) ) return 1;

	char* data = new char[len+1];

	if( g.getBytes((byte*)data, len) ) {
		delete[] data;
		return 2;
	}

	data[len] = 0;
	str = data;
	delete[] data;

	return 0;
}

#endif /* GETTER_HPP_ */
