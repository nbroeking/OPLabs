#include <io/UnixAddress.hpp>

#include <cstring>

#include <unistd.h>

namespace io {
using namespace std;

void UnixAddress::unlink() {
    ::unlink(m_addr.sun_path);
}

UnixAddress::UnixAddress(const char* address) {
    strncpy(m_addr.sun_path, address, sizeof(m_addr.sun_path));
    m_addr.sun_path[sizeof(m_addr.sun_path)-1] = 0;
    m_addr.sun_family = AF_UNIX;
}

struct sockaddr* UnixAddress::raw() const {
    return (struct sockaddr*)&m_addr;
}

socklen_t UnixAddress::rawlen() const {
    return sizeof(m_addr.sun_family) + strlen(m_addr.sun_path);
}

}
