#include <log/LogManager.hpp>

namespace logger {
    LogManager* me;
    LogManager& LogManager::instance() {
        if( ! me ) me = new LogManager();
        return * me;
    }
}
