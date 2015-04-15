#ifndef LOGCONTEXT_HPP_
#define LOGCONTEXT_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * LogContext.hpp: <description>
 */

#include <string>
#include <cstdarg>
#include <cstdio>
#include <prelude.hpp>

#include <io/BaseIO.hpp>
#include <io/StringWriter.hpp>
#include <os/Mutex.hpp>

namespace logger {

class LogManager;

/**
 * @brief Level to log by. Is given a color, name and level.
 */
class LogLevel {
public:
    /**
     * @brief Create a new LogLevel with the given color, name and level
     * 
     * @param color The color.
     * @param name The name
     * @param lev The level
     * @param bold True if should be bold
     */
    inline LogLevel(int color, const char* name, int lev, bool bold=1) {
        char txt[2048];
        level = lev;
        snprintf(txt, sizeof(txt), "\e[%02d;%02dm",
            bold?1:0, color);
        this->esc = txt;
        this->text = name;

        register_self();
    }

    /**
     * @brief More raw than the above, but more powerful.
     * 
     * @param esc The escape code to print the format
     * @param name The name
     * @param lev The level
     */
    inline LogLevel(const char* esc, const char* name, int lev) {
        this->esc = std::string(esc);
        text = name;
        level = lev;

        register_self();
    }

    std::string esc;
    std::string text;
    int level;

    /**
     * @brief Prints the level
     * 
     * @param out <stuff>
     * @param color <stuff>
     */
    inline void printlevel( io::StringWriter& out, bool color ) const {
        if( color ) {
            out.printf("%s%s\e[0m", esc.c_str(), text.c_str());
        } else {
            out.printf("%s",  text.c_str());
        }
    }

    static LogLevel* getLogLevelByName(const char* name);

private:
    void register_self();
};

class HollowLock {
public:
    inline void lock() {};
    inline void unlock() {};
    inline int try_lock() {return 1;}
};

/**
 * @brief A context to a log. Use this class for all Logging.
 * @see LogManager
 */
class LogContext {
public:
#ifdef ENVIRONMENT_devel
    static os::Mutex shared_lock;
#else
    static HollowLock shared_lock;
#endif

    /**
     * @brief Set the minimum allowable log level. Default for
     * devel environment is DEBUG, for release it is INFO
     *
     * @param lev the minimum level
     */
    void setMinimumLevel( const LogLevel& lev ) ;

    /**
     * @brief Print a string to the log using a va_list
     */
    void vprintf(const LogLevel& lev, const char* fmt, va_list ls) ;

    /**
     * @brief Like to above, but using vargs instead of a va_list
     */
    void printf(const LogLevel& lev, const char* fmt, ...) ;

    /**
     * @brief like the above, but append a newline to the end by default.
     */
    void printfln(const LogLevel& lev, const char* fmt, ... ) ;

    /**
     * @brief Print the hex representation of a byte array
     * @param lev LogLevel to print out to
     * @param bytes The byte array
     * @param len The length of the byte array
     */
    void printHex(const LogLevel& lev, const byte* bytes, size_t len) ;

    /**
     * @return The BaseIO this LogContext is printing to
     */
    inline io::BaseIO* getBaseIO() {
        return out.getBaseIO();
    }

    /**
     * @brief Enable or disable this LogContext
     * @param enabled True to enable log, false to disable
     */
    inline void setEnabled( bool enabled ) {
        this->enabled = enabled ;
    }

    /**
     * @return True if this log is enabled, false otherwise
     */
    inline bool isEnabled() {
        return this->enabled;
    }

    /**
     * @brief Redirect this log to another output stream
     * @param next The new output stream
     * @param color True to include color, false otherwise
     */
    inline void redirect(io::BaseIO* next, bool color) {
        out.setBaseIO(next);
        this->color = color;
    }

    static void unlock();
private:
    void log16hex( const LogLevel& lev, const byte* bytes, size_t len ) ;
    friend class LogManager;

    LogContext(const std::string& maj, const std::string& minor) ;
    LogContext(const LogContext&) ;
    void operator=(const LogContext&) ;

    int min_lev;
    io::StringWriter out;

    std::string major_context;
    std::string minor_context;

    bool enabled;
    bool color;
};

}

extern const logger::LogLevel TRACE;
extern const logger::LogLevel DEBUG;
extern const logger::LogLevel INFO;
extern const logger::LogLevel SUCCESS;

extern const logger::LogLevel WARN;
extern const logger::LogLevel ERROR;
extern const logger::LogLevel FATAL;

#endif /* LOGCONTEXT_HPP_ */
