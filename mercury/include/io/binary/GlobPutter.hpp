#ifndef GLOBPUTTER_HPP_
#define GLOBPUTTER_HPP_

/*
 * Author: jrahm
 * created: 2014/11/09
 * GlobPutter.hpp: <description>
 */

#include <io/binary/Putter.hpp>

namespace io {

/**
 * @brief A putter that puts bytes to a byte array.
 */
class GlobPutter : public Putter {
public:
    inline GlobPutter(){}

    virtual inline void putByte( byte b ) OVERRIDE {
        /* Push to the back */
        vec.push_back( b );
    }

    /**
     * @brief Grab the glob built by the GlobPutter
     * 
     * @param out_len a reference to the length of the glob returned
     * 
     * @return a byte array containing what was written to this Putter on success
     *          otherwise, on error, NULL is returned and out_len remains the same
     */
    virtual byte* serialize( ssize_t& out_len ); 

private:
    /* use a vector since we don't really need insertion
     * or deletion */
    std::vector<byte> vec;
};

}


#endif /* GLOBPUTTER_HPP_ */
