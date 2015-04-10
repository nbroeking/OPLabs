#include <mercury/Mercury.hpp>
#include "private/Mercury.hpp"

namespace mercury {

class private_Proxy: public Proxy {
    void start(const Config& cfg, Callback* callback) {
        (void)callback;

        m_merc.setId(cfg.mercury_id);
        m_merc.start();
    }
private:
    Mercury m_merc;
};

private_Proxy* g_merc_instnace;

Proxy& ProxyHolder::instance() {
    if(!g_merc_instnace) {
        g_merc_instnace = new private_Proxy();
    }
    return *g_merc_instnace;
}
    
}
