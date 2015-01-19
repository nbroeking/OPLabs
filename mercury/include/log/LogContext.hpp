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

    inline void printlevel( FILE* out ) const {
        fprintf(out, "%s%s\e[0m", esc.c_str(), text.c_str());
    }
};

class LogContext {
public:
    LogContext(const std::string& maj, const std::string& minor) :
        min_lev( 0 ),
        major_context( maj ),
        minor_context( minor ),
        enabled( false ) {
            out = stdout;
        }

    inline void vprintf(const LogLevel& lev, const char* fmt, va_list ls) {
        fprintf( out, "[%s|%s][", major_context.c_str(), minor_context.c_str() );
        lev.printlevel( out );
        fprintf( out, "] - %s", lev.esc.c_str());

        vfprintf( out, fmt, ls );

        fprintf(out, "\e[00m");
    }

    inline void printf(const LogLevel& lev, const char* fmt, ...) {
        if( ! enabled ) return ;
        if( lev.level < min_lev ) return ;       

        va_list ls;    
        va_start( ls, fmt );
        vprintf(lev, fmt, ls);
        va_end( ls );
    }

    inline void printfln(const LogLevel& lev, const char* fmt, ... ) {
        va_list ls;    
        va_start( ls, fmt );
        vprintf(lev, fmt, ls);
        va_end(ls);
        fprintf(out, "\n");
    }

    inline void printHex(const LogLevel& lev, const byte* bytes, size_t len) {
        while( len > 16 ) {
            log16hex( lev, bytes, 16 );
            len -= 16;
            bytes += 16;
        }

        log16hex( lev, bytes, len );
    }

    void setEnabled( bool enabled ) {
        this->enabled = enabled ;
    }

    void redirect(FILE* next) {
        out = next;
    }

private:
    char nybtochr( byte b ) {
        if ( b < 10 ) return b + 0x30;
        return (b-10) + 0x61;
    }

    void log16hex( const LogLevel& lev, const byte* bytes, size_t len ) {
        char buf1[16 * 3 + 1] = {0};
        char buf2[16 + 1] = {0};
        buf1[16*3] = 0;
        buf2[16] = 0;


        for ( size_t i = 0 ; i < len; ++ i ) {
            buf1[i*3 + 0] = ' ';
            buf1[i*3 + 1] = nybtochr((bytes[i]&0xF0) >> 4);
            buf1[i*3 + 2] = nybtochr(bytes[i]&0x0F);
            
            if( (isprint((char)bytes[i]) && ! isspace((char)bytes[i])) ||
                bytes[i] == ' ' ) {
                buf2[i] = bytes[i];
            } else {
                buf2[i] = '.';
            }
                
        }

        printfln(lev, "%-52s|    %-22s", buf1, buf2);
    }

private:
    int min_lev;
    FILE* out;

    std::string major_context;
    std::string minor_context;

    bool enabled;
};

}

extern logger::LogLevel TRACE;
extern logger::LogLevel DEBUG;
extern logger::LogLevel INFO;
extern logger::LogLevel SUCCESS;

extern logger::LogLevel WARN;
extern logger::LogLevel ERROR;
extern logger::LogLevel FATAL;

#endif /* LOGCONTEXT_HPP_ */
