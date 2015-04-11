#include <log/LogContext.hpp>
#include <io/FilePointer.hpp>

#include <os/Atomic.hpp>
#include <stdio.h>

const logger::LogLevel TRACE(34, "TRACE", 1, true);
const logger::LogLevel DEBUG(35, "DEBUG", 5, true);

const logger::LogLevel INFO("\e[01;37m", "INFO", 10);
const logger::LogLevel SUCCESS(32, "SUCCESS", 10, true);

const logger::LogLevel WARN(33, "WARN", 15, true);
const logger::LogLevel ERROR(31, "ERROR", 20, true);
const logger::LogLevel FATAL(31, "FATAL", 15, false);

using namespace std;
using namespace os;

#include <map>

namespace logger {

AtomicHolder<map<std::string, LogLevel*> >& get_g_log_level_db() {
    static AtomicHolder<map<std::string, LogLevel*> > ret;
    return ret;
}

void LogLevel::register_self() {
    Atomic<map<string, LogLevel*> > m_atomic = get_g_log_level_db().get();
    (*m_atomic)[this->text] = this;
}

LogLevel* LogLevel::getLogLevelByName(const char* name) {
    Atomic<map<string, LogLevel*> > m_atomic = get_g_log_level_db().get();
    return (*m_atomic)[name];
}

io::FilePointer p_stdout(stdout);

LogContext::LogContext(const string& maj, const string& minor) :
    min_lev( 0 ),
    major_context( maj ),
    minor_context( minor ),
    enabled( false ),
    color( true )
    {
        this->redirect(&p_stdout, true);
    }

void LogContext::setMinimumLevel( const LogLevel& lev ) {
    min_lev = lev.level;
}

#ifdef ENVIRONMENT_devel
os::Mutex LogContext::shared_lock;
#else
HollowLock LogContext::shared_lock;
#endif
void LogContext::vprintf(const LogLevel& lev, const char* fmt, va_list ls) {
    if( ! enabled ) return ;
    if( lev.level < min_lev ) return ;       

    shared_lock.lock();
    if( color ) {
        out.printf( "[%s%s\e[00m|%s%s\e[00m][",
            lev.esc.c_str(),
            major_context.c_str(),
            lev.esc.c_str(),
            minor_context.c_str() );

        lev.printlevel( out, color );
        out.printf( "] - %s", lev.esc.c_str());
    } else {
        out.printf( "[%s|%s][",
            major_context.c_str(),
            minor_context.c_str() );

        lev.printlevel( out, color );
        out.printf( "] - " );
    }

    out.vprintf( fmt, ls );

    out.printf("\e[00m");
    shared_lock.unlock();
}

void LogContext::printf(const LogLevel& lev, const char* fmt, ... ) {
    if( ! enabled ) return ;
    if( lev.level < min_lev ) return ;       

    va_list ls;    
    va_start( ls, fmt );
    vprintf(lev, fmt, ls);
    va_end( ls );
}

void LogContext::printfln(const LogLevel& lev, const char* fmt, ... ) {
    if( ! enabled ) return ;
    if( lev.level < min_lev ) return ;       

    va_list ls;    
    va_start( ls, fmt );
    vprintf(lev, fmt, ls);
    va_end(ls);
    out.printf("\n");
}

void LogContext::printHex(const LogLevel& lev, const byte* bytes, size_t len) {
    if(len > 1024) {
        this->printfln(lev, "First 1024 bytes of array %lu long:", len);
        len = 1024;
    }
    while( len > 16 ) {
        log16hex( lev, bytes, 16 );
        len -= 16;
        bytes += 16;
    }

    log16hex( lev, bytes, len );
}

char nybtochr( byte b ) {
    if ( b < 10 ) return b + 0x30;
    return (b-10) + 0x61;
}

void LogContext::log16hex( const LogLevel& lev, const byte* bytes, size_t len ) {
    char buf1[16 * 3 + 1] = {0};
    char buf2[18 + 1] = {0};
    buf1[16*3] = 0;
    buf2[18] = 0;
    buf2[0] = buf2[17] = '|';


    for ( size_t i = 0 ; i < len; ++ i ) {
        buf1[i*3 + 0] = ' ';
        buf1[i*3 + 1] = nybtochr((bytes[i]&0xF0) >> 4);
        buf1[i*3 + 2] = nybtochr(bytes[i]&0x0F);
        
        if( (isprint((char)bytes[i]) && ! isspace((char)bytes[i])) ||
            bytes[i] == ' ' ) {
            buf2[i+1] = bytes[i];
        } else {
            buf2[i+1] = '.';
        }
            
    }

    printfln(lev, "%-52s    %-22s", buf1, buf2);
}


}


