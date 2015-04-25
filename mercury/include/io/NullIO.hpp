#ifndef NULLIO_HPP_
#define NULLIO_HPP_

/*
 * Author: jrahm
 * created: 2015/04/11
 * NullIO.hpp: <description>
 */

#include <io/BaseIO.hpp>

namespace io {

class NullIO: public BaseIO {
public:
    inline ssize_t read(byte* out, size_t len){
        (void) out;
        return len; };
    inline ssize_t write(const byte* out, size_t len){
        (void) out;
        return len; };
    inline int close() {return 0;}
    
    static NullIO& instance();
    NullIO();
};

}

#endif /* NULLIO_HPP_ */
