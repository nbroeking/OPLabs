#include <io/MuxIO.hpp>

#include <Prelude>

namespace io {

using namespace std;
using namespace os;

ssize_t MuxIO::write(const byte* in, size_t len) {
    ssize_t min = len + 1;
    Atomic<set<io::BaseIO*> > base_ios = m_base_ios.get();
    set<io::BaseIO*>::iterator itr;

    FOR_EACH(itr, *base_ios) {
        ssize_t o = (*itr)->write(in, len);
        if(o < min) {
            min = o;
        }
    }

    return min;
}

int MuxIO::close() {
    Atomic<set<io::BaseIO*> > base_ios = m_base_ios.get();
    std::set<io::BaseIO*>::iterator itr;

    FOR_EACH(itr, *base_ios) {
        (*itr)->close();
    }
    return 0;
}

}
