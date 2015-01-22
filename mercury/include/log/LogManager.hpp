#ifndef LOGMANAGER_HPP_
#define LOGMANAGER_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * LogManager.hpp: <description>
 */

#include<log/LogContext.hpp>
#include<os/Mutex.hpp>
#include<os/ScopedLock.hpp>

#include <map>
#include <set>

namespace logger {

class LogManager {
public:
    typedef std::map< std::string, LogContext* >::iterator iterator;


    inline LogContext& getLogContext(const std::string& maj, const std::string& min) {
        os::ScopedLock __sl(m_mutex);
        return _getLogContext(maj, min);
    }

    inline void getLogsForMajor(const std::string& maj, iterator& begin, iterator& end) {
        os::ScopedLock __sl(m_mutex);
        return _getLogsForMajor(maj, begin, end);
    }

    inline void enableAllForMajor(const std::string& maj) {
        os::ScopedLock __sl(m_mutex);
        return _enableAllForMajor(maj);
    }

    void setEnableByDefault( bool def ) {
        enable_default = def;
    }

    static LogManager& instance() ;
private:

    LogContext& _getLogContext(const std::string& maj, const std::string& min) ;

    void _getLogsForMajor(const std::string& maj, iterator& begin, iterator& end) ;

    void _enableAllForMajor(const std::string& maj) ;

    inline LogManager(): enable_default(false) {}

    typedef std::map< std::string, std::map< std::string, LogContext* > > map_type ;

    bool enable_default;
    std::set< std::string > enabled_majors;
    map_type m_db;
    os::Mutex m_mutex;
};

}

#endif /* LOGMANAGER_HPP_ */
