#ifndef GETTER_HPP_
#define GETTER_HPP_

/*
 * Author: jrahm
 * created: 2014/11/08
 * Getter.hpp: <description>
 */

#include <prelude.hpp>

#include <vector>
#include <cstdlib>

#include <string>
#include <algorithm>
#include <cstdio>

namespace io {

class GetterUnderflowException: public Exception {
public:
    GetterUnderflowException( const char* exc ) : Exception(exc) {}
};

/**
 * @brief Abstracted class for decoding of binary data
 *
 * The Getter class is a class designed to take a block of data
 * and read from it as a stream. The Getter is able to read
 * little endian and big endian integers as well as byte stringo.
 *
 * The Getter is extensible. To make an object binary decodable, one
 * must only implement the function <code>int getObject(Getter&,T&)</code>
 *
 * @see getObject(Getter&,string&)
 */
class Getter {
public:
    /**
     * @brief construct a new getter with the binary blob <code>blob</code>
     *
     * @param blob the blob to decode
     * @param blob_len the length of blob
     *
     * This will create a new getter which will pull data from the binary blob
     * provided
     */
	Getter( byte*& blob, size_t blob_len ) :
		m_blob(blob), m_blob_len( blob_len ), m_cursor(0) {
			blob = NULL;
		}

    /**
     * @brief Read just a singleton byte from this Getter and put it it <code>b</code>
     * @param b the reference to store the value
     * @return 0 on success, 1 if the Getter is out of bytes
     */
	virtual inline int getByte( byte& b ) {
		if ( m_cursor >= m_blob_len ) {
            char buf[1024];
            snprintf(buf, sizeof(buf), "Gettor underflow at position %lu", m_cursor);
            throw GetterUnderflowException(buf);
		}
		b = m_blob[m_cursor ++];
		return 0;
	}
	
    /**
     * @brief get a 16 bit integer in little endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline int getInt16le( uint16_t& i ) {
		byte high, low;
		int rc = getByte( low ) || getByte( high ) ;
		i = high << 8 | low ;
		return rc;
	}

    /**
     * @brief get a 16 bit integer in big endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline int getInt16be( uint16_t& i ){
		byte high, low;
		int rc = getByte( high ) || getByte( low ) ;
		i = high << 8 | low ;
		return rc;
	}

    /**
     * @brief get a 32 bit integer in little endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline int getInt32le( uint32_t& i ) {
		uint16_t high, low;
		int rc = getInt16le( low ) || getInt16le( high ) ;
		i = high << 16 | low ;
		return rc;
	}

    /**
     * @brief get a 32 bit integer in big endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline int getInt32be( uint32_t& i ) {
		uint16_t high, low;
		int rc = getInt16be( high ) || getInt16be( low ) ;
		i = high << 16 | low ;
		return rc;
	}

    /**
     * @brief get a 64 bit integer in little endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline int getInt64le( uint64_t& i ) {
		uint32_t high, low;
		int rc = getInt32le( low ) || getInt32le( high ) ;
		i = ((uint64_t)high) << 32 | low ;
		return rc;
	}

    /**
     * @brief get a 64 bit integer in big endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline int getInt64be( uint64_t& i ) {
		uint32_t high, low;
		int rc = getInt32be( high ) || getInt32be( low ) ;
		i = ((uint64_t)high) << 32 | low ;
		return rc;
	}

    /**
     * @brief get a raw array of bytes from the getter
     * @param into the bytes to read into
     * @param len the number of bytes to read
     *
     * If there is an error, this function will not
     * consume any input from the getter.
     *
     * @return 0 on success, 1 on error
     */
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

/**
 * @brief Generalization of the Get function for the Getter.
 * @param g the getter to read off of
 * @param t the value to read into
 *
 * Implement a version of this function to turn an object into
 * a gettable object.
 */
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
