#include <io/UnixAddress.hpp>

#include <cstring>

#include <unistd.h>

namespace io {
using namespace std;

void UnixAddress::unlink() {
    ::unlink(m_addr.sun_path);
}

UnixAddress::~UnixAddress() {
    this->unlink();
}

UnixAddress::UnixAddress(const char* address) {
    strncpy(m_addr.sun_path, address, sizeof(m_addr.sun_path));
    m_addr.sun_path[sizeof(m_addr.sun_path)-1] = 0;
    m_addr.sun_family = AF_UNIX;
}

socklen_t UnixAddress::rawlen() const {
    return sizeof(m_addr.sun_family) + strlen(m_addr.sun_path);
}

std::string UnixAddress::toString() const {
    return m_addr.sun_path;
}

}
