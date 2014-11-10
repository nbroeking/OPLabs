#ifndef STREAMGETTER_HPP_
#define STREAMGETTER_HPP_

/*
 * Author: jrahm
 * created: 2014/11/08
 * StreamGetter.hpp: <description>
 */

#include <cstdio>
#include <io/binary/Getter.hpp>
#include <io/BaseIO.hpp>

namespace io {

class StreamGetter: public Getter {
public:
	inline StreamGetter( BaseIO*& base, bool take=false ) :
		input( base ) {
            if( take ) base = NULL;
            m_is_owner = take;
			m_blob = new byte[m_buffer_size = 1024];
		}
    inline StreamGetter( BaseIO* base ) : input(base){
        m_is_owner = false;
		m_blob = new byte[m_buffer_size = 1024];
    }

	virtual int getByte( byte& out ) OVERRIDE {
		if ( m_cursor >= m_blob_len ) {
			do {
				ssize_t ret;
				ret = input->read( m_blob, m_buffer_size );
				if( ret <= -1 ) return 1; 
	
				m_cursor = 0;
				m_blob_len = ret;
			} while ( m_blob_len == 0 ) ;
		}
		
		out = m_blob[m_cursor++];
		return 0;
	}

    virtual inline int getBytes( byte* into, size_t len ) {
        int rc;
        for( size_t i = 0; i < len; ++ i ) {
            rc = getByte(into[i]);
            if( rc ) { return rc; }
        }
        return 0;
    }

	virtual inline ~StreamGetter() {
        if( m_is_owner ) delete input;
		delete[] m_blob ;
	}

private:
	uint m_buffer_size;
    bool m_is_owner;
	BaseIO* input;
};

}

#endif /* STREAMGETTER_HPP_ */
