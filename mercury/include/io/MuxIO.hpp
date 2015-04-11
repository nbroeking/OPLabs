#ifndef MUXIO_HPP_
#define MUXIO_HPP_

/*
 * Author: jrahm
 * created: 2015/04/10
 * MuxIO.hpp: <description>
 */

#include <io/BaseIO.hpp>
#include <os/Atomic.hpp>
#include <set>

namespace io {

class MuxIO: public BaseIO {
public:
    virtual ssize_t read(byte* out, size_t len) {
        (void)out; (void)len;
        return -1;
    };

    virtual ssize_t write(const byte* in, size_t len);

    virtual int close();

    inline void addBaseIO(BaseIO* ptr) {
        m_base_ios.get()->insert(ptr);
    }

    inline void removeBaseIO(BaseIO* ptr) {
        m_base_ios.get()->erase(ptr);
    }

private:
    os::AtomicHolder<std::set<BaseIO*> > m_base_ios;
};

}

#endif /* MUXIO_HPP_ */
