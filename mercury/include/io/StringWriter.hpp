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

namespace io {

class StringWriter {
public:
    inline StringWriter( BaseIO*& io_base ) {
        stealBaseIO( io_base );
    }
        
    inline StringWriter( ) {}

    inline void stealBaseIO( BaseIO*& base ) {
        m_is_owner = true;
        m_base_io = base;
        base = NULL;
    }

    inline void setBaseIO( BaseIO* base ) {
        m_is_owner = false;
        m_base_io = base;
    }

    inline void print( const std::string& str ) {
        m_base_io->write( (const byte*)str.c_str(), str.size() );
    }

    inline void printf( const char* fmt, ... ) {
        char* out ;

        va_list a;
        va_start(a, fmt);
        vasprintf( &out, fmt, a );
        print( out );
        free( out );
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
