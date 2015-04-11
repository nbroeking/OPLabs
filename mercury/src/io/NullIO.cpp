#include <io/NullIO.hpp>

namespace io {

NullIO* g_null_io;

NullIO::NullIO(){}

NullIO& NullIO::instance() {
    if(!g_null_io) g_null_io = new NullIO();
    return *g_null_io;
}

}
