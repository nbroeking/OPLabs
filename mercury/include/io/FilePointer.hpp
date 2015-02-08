#ifndef FILEPONINTER_HPP_
#define FILEPONINTER_HPP_

/*
 * Author: jrahm
 * created: 2015/01/19
 * FilePoninter.hpp: <description>
 */

#include <io/BaseIO.hpp>
#include <cstdio>

namespace io {

class FilePointer : public BaseIO {
public:
    inline FilePointer(FILE* raw) {
        m_fp = raw;
    }

    FILE* raw() { return m_fp; }

    virtual ssize_t write( const byte* in, size_t len ) OVERRIDE {
        ssize_t ret;
        ret = fwrite( in, 1, len, m_fp );
        fflush(m_fp);
        return ret;
    }

    virtual ssize_t read( byte* out, size_t len ) OVERRIDE {
        return fread( out, 1, len, m_fp );
    }

    virtual int close() {
        return fclose(m_fp);
    }

private:
    FILE* m_fp;
};

}

#endif /* FILEPONINTER_HPP_ */
