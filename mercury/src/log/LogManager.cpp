#include <log/LogManager.hpp>

namespace logger {
LogManager* me;
LogManager& LogManager::instance() {
    if( ! me ) me = new LogManager();
    return * me;
}


LogContext& LogManager::_getLogContext(const std::string& maj, const std::string& min) {

    if( m_db[maj][min] == NULL ) {
        LogContext* ctx = new LogContext(maj, min);
        m_db[maj][min] = ctx;

        ctx->setEnabled(enable_default);
        if ( enabled_majors.find(maj) != enabled_majors.end() ) {
            ctx->setEnabled(true);
        }
    }

    return * m_db[maj][min];
}

void LogManager::_getLogsForMajor(const std::string& maj, iterator& begin, iterator& end) {

    std::map< std::string, LogContext* >& m = m_db[maj];
    begin = m.begin();
    end = m.end();
}

void LogManager::_enableAllForMajor(const std::string& maj) {

    iterator cur;
    iterator end;

    _getLogsForMajor(maj, cur, end);
    for( ; cur != end; ++ cur ) {
        cur->second->setEnabled( true );
    }

    enabled_majors.insert(maj);
}
}
