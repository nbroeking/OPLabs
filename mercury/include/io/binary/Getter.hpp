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
     * @brief Read just a singleton byte from this Getter and put it it <code>b</code>
     * @param b the reference to store the value
     * @return 0 on success, 1 if the Getter is out of bytes
     */
	virtual byte getByte( ) = 0; 
	
    /**
     * @brief get a 16 bit integer in little endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline u16_t getInt16le() {
		byte high, low;
		low = getByte();
		high = getByte() ;
		return high << 8 | low ;
	}

    /**
     * @brief get a 16 bit integer in big endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline u16_t getInt16be(){
		byte high, low;
		high = getByte() ;
		low = getByte();
		return high << 8 | low ;
	}

    /**
     * @brief get a 32 bit integer in little endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline u32_t getInt32le() {
		u16_t high, low;
		low = getInt16le() ; high = getInt16le() ;
		return high << 16 | low ;
	}

    /**
     * @brief get a 32 bit integer in big endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline u32_t getInt32be() {
		uint16_t high, low;
		high = getInt16be() ; low = getInt16be() ;
		return high << 16 | low ;
	}

    /**
     * @brief get a 64 bit integer in little endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline u64_t getInt64le() {
		u32_t high, low;
		low = getInt32le() ; high = getInt32le() ;
		return ((u64_t)high) << 32 | low ;
	}

    /**
     * @brief get a 64 bit integer in big endian form
     * @param i where to store the value
     * @return 0 on success, 1 on error
     */
	virtual inline u64_t getInt64be() {
		return ((u64_t)getInt32be()) << 32 | getInt32be() ;
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
	virtual inline void getBytes( byte* into, size_t len ) {
        for( size_t i = 0 ; i < len ; ++ i )
            into[i] = getByte();
	}

	virtual inline ~Getter(){}
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
int getObject( io::Getter& g, T& t );

inline void getObject( io::Getter& g, byte& b ) {
    b = g.getByte();
}

template <>
inline int getObject( io::Getter& g, std::string& str ) {
	uint32_t len;

	len = g.getInt32le();

	char* data = new char[len+1];

	try {
    	g.getBytes((byte*)data, len);
	} catch ( Exception& e ) {
		delete[] data;
		throw e;
	}

	data[len] = 0;
	str = data;
	delete[] data;

	return 0;
}

#endif /* GETTER_HPP_ */
