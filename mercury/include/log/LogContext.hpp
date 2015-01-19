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

#if defined(ENVIRONMENT_release)
#define DEFUALT_MIN_LOG 3
#else
#define DEFUALT_MIN_LOG 1
#endif

namespace logger {

class LogLevel {
public:
    inline LogLevel(int color, const char* name, int lev, bool bold=1) {
        char txt[2048];
        level = lev;
        snprintf(txt, sizeof(txt), "\e[%02d;%02dm",
            bold?1:0, color);
        this->esc = txt;
        this->text = name;
    }

    inline LogLevel(const char* esc, const char* name, int lev) {
        this->esc = std::string(esc);
        text = name;
        level = lev;
    }

    std::string esc;
    std::string text;
    int level;

    inline void printlevel( FILE* out, bool color ) const {
        if( color ) {
            fprintf(out, "%s%s\e[0m", esc.c_str(), text.c_str());
        } else {
            fprintf(out, "%s",  text.c_str());
        }
    }
};

class LogContext {
public:
    LogContext(const std::string& maj, const std::string& minor) ;

    void setMinimumLevel( const LogLevel& lev ) ;

    void vprintf(const LogLevel& lev, const char* fmt, va_list ls) ;

    void printf(const LogLevel& lev, const char* fmt, ...) ;

    void printfln(const LogLevel& lev, const char* fmt, ... ) ;

    void printHex(const LogLevel& lev, const byte* bytes, size_t len) ;

    inline void setEnabled( bool enabled ) {
        this->enabled = enabled ;
    }

    inline void redirect(FILE* next, bool color) {
        out = next;
        color = color;
    }

private:
    void log16hex( const LogLevel& lev, const byte* bytes, size_t len ) ;

private:
    int min_lev;
    FILE* out;

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
