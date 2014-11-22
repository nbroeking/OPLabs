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

/**
 * @brief An abstraction fo the Getter class to operate on streams
 *
 * @see getter
 */
class StreamGetter: public Getter {
public:
    /**
     * Construct a StreamGetter from the given BaseIO.
     * @param base the BaseIO to use
     * @param take should this take ownership of <code>base</code>
     *
     * If take is true, then this object will take control of
     * <code>base</code> and assume responsibility to delete
     * the object when it goes out of scope. If this is the
     * case, by pointer-reference notation, the original pointer
     * to base will be set to NULL.
     */
	inline StreamGetter( BaseIO*& base, bool take=false ) :
		input( base ) {
            if( take ) base = NULL;
            m_is_owner = take;
			m_blob = new byte[m_buffer_size = 1024];
		}

    /**
     * Contstruct a StreamGetter from the given BaseIO
     * @param base the BaseIO to use
     *
     * Assumes no ownership responsibilities, unlike the above
     */
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
