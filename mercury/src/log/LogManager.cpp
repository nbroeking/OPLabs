#include <log/LogManager.hpp>

#include <io/FilePointer.hpp>

namespace logger {
LogManager* me;
LogManager& LogManager::instance() {
    if( ! me ) {
        me = new LogManager();
        me->m_out.addBaseIO(new io::FilePointer(stdout));
    }
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
        } else if ( disabled_majors.find(maj) != disabled_majors.end() ) {
            ctx->setEnabled(false);
        }

        ctx->redirect(&m_out, m_color);
    }

    return * m_db[maj][min];
}

void LogManager::_getLogsForMajor(const std::string& maj, iterator& begin, iterator& end) {
    std::map< std::string, LogContext* >& m = m_db[maj];
    begin = m.begin();
    end = m.end();
}

void LogManager::_setLogLevelForAll(const LogLevel& level) {
    setDefaultLevel(level);
    map_type::iterator itr;
    for( itr = m_db.begin(); itr != m_db.end(); ++ itr ) {
        _enableAllForMajor(itr->first, true);
        iterator cur;
        iterator end;
    
        _getLogsForMajor(itr->first, cur, end);
        for( ; cur != end; ++ cur ) {
            cur->second->setMinimumLevel( level );
        }
    }
}

void LogManager::_enableAllForMajor(const std::string& maj, bool yes) {

    iterator cur;
    iterator end;


    if(yes) {
        disabled_majors.erase(maj);
        enabled_majors.insert(maj);
    } else {
        enabled_majors.erase(maj);
        disabled_majors.insert(maj);
    }

    _getLogsForMajor(maj, cur, end);
    for( ; cur != end; ++ cur ) {
        cur->second->setEnabled(yes);
    }
}

void LogManager::_logEverything() {
    map_type::iterator itr;
    for( itr = m_db.begin(); itr != m_db.end(); ++ itr ) {
        _enableAllForMajor(itr->first, true);
        iterator cur;
        iterator end;
    
        _getLogsForMajor(itr->first, cur, end);
        for( ; cur != end; ++ cur ) {
            cur->second->setMinimumLevel( defaultLevel );
        }
    }
}

void LogManager::addLogOutput(io::BaseIO* next, bool color) {
    m_out.addBaseIO(next);
    m_color = color;
}

void LogManager::removeLogOutput(io::BaseIO* next) {
    m_out.removeBaseIO(next);
}

}
