#include <test/Tester.hpp>

using namespace logger;
using namespace std;

namespace test {

LogLevel PASSED(32, "PASSED", 10);
LogLevel FAILED(31, "FAILED", 10);

Tester* p_instance = NULL;

Tester::Tester(const char* name):
    m_log( LogManager::instance().getLogContext("Tester", name) ) {
    m_tests_failed = 0;
    m_tests_passed = 0;
}

Tester& Tester::instance() {
    return * p_instance;
}

void Tester::init( const char* name, int argc, char** argv ) {
    (void) argc; (void) argv;
    p_instance = new Tester(name);
    LogManager::instance().logEverything();
}

int Tester::exit() {
    bool rc = m_tests_failed > 0;

    if ( rc ) {
        vector<string>::iterator itr;
        for( itr = failed.begin(); itr != failed.end() ; ++ itr ) {
            m_log.printfln(ERROR, "Failed - %s", itr->c_str());
        }
        m_log.printfln(FATAL, "%d/%d tests passed.", m_tests_passed, (m_tests_passed+m_tests_failed));
    } else {
        m_log.printfln(SUCCESS, "All tests passed");
    }

    return rc;
}

#define PURPLE  "\e[1;35m"
int Tester::testBool( const char* name, bool b, const char* function, const char* file, size_t line ) {
    if( b ) {
        m_log.printfln(PASSED, "%-20s - "PURPLE"@(%s %s:%d)", name, function, file, line);
        m_tests_passed ++;
    } else {
        m_log.printfln(FAILED, "%-20s - "PURPLE"@(%s %d:%d)", name, function, file, line);
        m_tests_failed ++;
        failed.push_back(name);
    }
    return 0;
}

}
