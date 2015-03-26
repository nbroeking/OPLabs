#ifndef BASE64PUTTER_HPP_
#define BASE64PUTTER_HPP_

/*
 * Author: jrahm
 * created: 2015/02/17
 * Base64Putter.hpp: <description>
 */

#include <io/binary/Putter.hpp>

extern "C" {
#include <3rdparty/b64/cencode.h>
}

#include <sstream>

namespace io {

#define BASE64_PUTTER_BUFFER_SIZE 128
#define BASE64_PUTTER_ENCODE_SIZE (BASE64_PUTTER_BUFFER_SIZE * 2)

class Base64Putter: public Putter {
public:
    Base64Putter() {
        base64_init_encodestate( &this->enc );
        buffer_cur = 0;
    }

    virtual void putByte(byte b) {
        buffer[buffer_cur ++] = b;

        if( buffer_cur == BASE64_PUTTER_BUFFER_SIZE ) {
            char out_block[ BASE64_PUTTER_ENCODE_SIZE ];
            size_t cnt = base64_encode_block( buffer, buffer_cur, out_block, &enc );
            out_block[cnt] = 0;
            out << out_block;

            buffer_cur = 0;
        }
    }

    inline std::string serialize() {
        std::string ret = out.str();
        char out_block[ BASE64_PUTTER_ENCODE_SIZE ];

        size_t cnt = base64_encode_block( buffer, buffer_cur, out_block, &enc );
        out_block[cnt] = 0;
        ret += out_block;

        cnt = base64_encode_blockend( out_block, &enc );
        out_block[cnt] = 0;
        ret += out_block;
        return ret;
    }


private:
    char buffer[ BASE64_PUTTER_BUFFER_SIZE ];
    size_t buffer_cur;

    base64_encodestate enc;
    std::stringstream out;
};

inline std::string simpleBase64Encode(const byte* input, size_t size) {
    Base64Putter putter;
    putter.putBytes(input,size);
    return putter.serialize();
}

}

#endif /* BASE64PUTTER_HPP_ */
