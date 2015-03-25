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
        ctx->setMinimumLevel( defaultLevel );
        if ( enabled_majors.find(maj) != enabled_majors.end() ) {
            ctx->setEnabled(true);
        }
    }

    printf("Returning %p\n", m_db[maj][min]);
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

void LogManager::_logEverything() {
    map_type::iterator itr;
    for( itr = m_db.begin(); itr != m_db.end(); ++ itr ) {
        _enableAllForMajor(itr->first);
        iterator cur;
        iterator end;
    
        _getLogsForMajor(itr->first, cur, end);
        for( ; cur != end; ++ cur ) {
            cur->second->setMinimumLevel( defaultLevel );
        }
    }
}

}
