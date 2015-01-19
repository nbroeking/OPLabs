#ifndef LOGMANAGER_HPP_
#define LOGMANAGER_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * LogManager.hpp: <description>
 */

#include<log/LogContext.hpp>

#include <map>

namespace logger {

class LogManager {
public:
    static LogManager& instance() ;

    LogContext& getLogContext(const std::string& maj, const std::string& min) {
        std::string key = maj + "+" + min;

        if( m_db[key] == NULL ) {
            m_db[key] = new LogContext(maj, min);
        }

        return * m_db[key];
    }
private:
    inline LogManager() {}

    typedef std::map< std::string, LogContext* > map_type ;
    map_type m_db;
};

}

#endif /* LOGMANAGER_HPP_ */
