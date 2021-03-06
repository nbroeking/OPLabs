#ifndef STRINGWRITER_HPP_
#define STRINGWRITER_HPP_

/*
 * Author: jrahm
 * created: 2014/11/07
 * StringWriter.hpp: <description>
 */

#include <io/BaseIO.hpp>
#include <string>

#include <cstdarg>
#include <cstring>
#include <cstdlib>

namespace io {

/**
 * @brief a class that abstracts writing to a socket
 */
class StringWriter {
public:
    /**
     * @brief Construct a new string writer.
     * @param io_base a pointer to the BaseIO to steal
     *
     * This function assumes ownership of the <code>io_base</code> 
     * passed into it
     */
    inline StringWriter( BaseIO*& io_base ) {
        stealBaseIO( io_base );
    }
        
    /**
     * @brief Construct a StringWriter with no BaseIO.
     */
    inline StringWriter( ): m_is_owner(false) {}

    /**
     * @brief Sets the BaseIO of this Writer and assumes ownership of it
     * @param base The base io to delegate to
     */
    inline void stealBaseIO( BaseIO*& base ) {
        m_is_owner = true;
        m_base_io = base;
        base = NULL;
    }

    /**
     * @brief Like the above, but does not assume ownership of the BaseIO
     * @param base the BaseIO te delegate to
     */
    inline void setBaseIO( BaseIO* base ) {
        m_is_owner = false;
        m_base_io = base;
    }

    /**
     * @brief Print a string to the BaseIO
     * @param str The string to print
     */
    inline void print( const std::string& str ) {
        m_base_io->write( (const byte*)str.c_str(), str.size() );
    }

    inline void print( const char* chrs ) {
        m_base_io->write( (const byte*)chrs, strlen(chrs) );
    }

    /**
     * @brief Acts like printf, but writes the contents to the BaseIO
     */
    inline void printf( const char* fmt, ... ) {

        va_list a;
        va_start(a, fmt);
        vprintf(fmt, a);
        va_end(a);
    }

    inline void vprintf( const char* fmt, va_list a ) {
        char* out ;
        vasprintf( &out, fmt, a );
        print( out );
        free( out );
    }

    inline BaseIO* getBaseIO() {
        return m_base_io;
    }

    inline ~StringWriter() {
        if( m_is_owner ) delete m_base_io;
    }

private:
    BaseIO* m_base_io;
    bool m_is_owner;
};

}

#endif /* STRINGWRITER_HPP_ */
