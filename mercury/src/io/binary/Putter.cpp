#include <io/binary/Putter.hpp>
#include <io/binary/GlobPutter.hpp>

using namespace std;

namespace io {

int Putter::putBytes( const byte* in, size_t len ) {
    for( size_t i = 0 ; i < len ; ++ i ) 
        putByte( i );
    return 0;
}

byte* GlobPutter::serialize( ssize_t& size ) {
    if( vec.size() == 0 ) {
        size = 0;
        return NULL;
    }

    byte* ret = new byte[size=vec.size()];
    copy( vec.begin(), vec.end(), ret );

    return ret;
}

}
