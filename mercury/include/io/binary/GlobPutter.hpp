#ifndef GLOBPUTTER_HPP_
#define GLOBPUTTER_HPP_

/*
 * Author: jrahm
 * created: 2014/11/09
 * GlobPutter.hpp: <description>
 */

#include <io/binary/Putter.hpp>

namespace io {

class GlobPutter : public Putter {
public:
    inline GlobPutter(){}

    virtual inline int putByte( byte b ) OVERRIDE {
        vec.push_back( b );
        return 0;
    }

    virtual byte* serialize( ssize_t& out_len ); 

private:
    std::vector<byte> vec;
};

}


#endif /* GLOBPUTTER_HPP_ */
