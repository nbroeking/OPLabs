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

#include <io/MuxIO.hpp>

#ifdef environment_RELEASE
#define DEFAULT_LEVEL INFO
#else
#define DEFAULT_LEVEL DEBUG
#endif

namespace logger {

/**
 * @brief A singleton class used to manage the various logs
 */
class LogManager {
public:
    typedef std::map< std::string, LogContext* >::iterator iterator;


    /**
     * @brief Return a log context with the specified major and minor strings.
     * @param maj The major string
     * @param min The minor string
     * @return A log context with the major and minor identifiers
     */
    inline LogContext& getLogContext(const std::string& maj, const std::string& min) {
        os::ScopedLock __sl(m_mutex);
        return _getLogContext(maj, min);
    }

    /**
     * @brief Get a begin and end interator for all log contexts with the given major string
     * @param maj The major string
     * @param begin Reference to store the beginning iterator
     * @param end Reference to store the end iterator
     */
    inline void getLogsForMajor(const std::string& maj, iterator& begin, iterator& end) {
        os::ScopedLock __sl(m_mutex);
        return _getLogsForMajor(maj, begin, end);
    }

    /**
     * @brief Enable all the logs for the major identifier
     * @param maj The major identifier to enable for
     */
    inline void enableAllForMajor(const std::string& maj, bool yes=true) {
        os::ScopedLock __sl(m_mutex);
        return _enableAllForMajor(maj, yes);
    }

    /**
     * @brief True if this manager should enable all LogContexts by default
     * @param def Whether or not to automatically enable
     */
    void setEnableByDefault( bool def ) {
        enable_default = def;
    }

    /**
     * @brief Returns the global instance
     * @return The global instace if this class
     */
    static LogManager& instance() ;

    /**
     * Sweeping declaration, enable all logs at every level at all times
     */
    void logEverything() {
        os::ScopedLock __sl(m_mutex);
        setEnableByDefault( true );
        setDefaultLevel(TRACE);
        _logEverything();
    }

    void addLogOutput(io::BaseIO* next, bool color);
    void removeLogOutput(io::BaseIO* next);

    void setDefaultLevel( const LogLevel& lev ) {
        this->defaultLevel = lev;
    }

    void setLogLevelForAll(const LogLevel& level) {
        os::ScopedLock __sl(m_mutex);
        _setLogLevelForAll(level);
    }

private:
    void _setLogLevelForAll(const LogLevel& level);

    LogContext& _getLogContext(const std::string& maj, const std::string& min) ;

    void _getLogsForMajor(const std::string& maj, iterator& begin, iterator& end) ;

    void _enableAllForMajor(const std::string& maj, bool yes) ;

    void _logEverything();

    inline LogManager(): enable_default(false), defaultLevel(DEBUG) {}

    typedef std::map< std::string, std::map< std::string, LogContext* > > map_type ;

    bool enable_default;
    std::set< std::string > enabled_majors;
    std::set< std::string > disabled_majors;
    LogLevel defaultLevel;
    map_type m_db;
    os::Mutex m_mutex;

    io::MuxIO m_out;
    bool m_color;
};

}

#endif /* LOGMANAGER_HPP_ */
