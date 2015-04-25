#ifndef TESTER_HPP_
#define TESTER_HPP_

/*
 * Author: jrahm
 * created: 2015/02/08
 * Tester.hpp: <description>
 */

#include <log/LogManager.hpp>
#include <vector>

namespace test {

#define TEST_BOOL(name, b) \
    test::Tester::instance().testBool(name, b, __FUNCTION__, __FILE__, __LINE__)

#define TEST_STR_EQ(name, a, b) \
    test::Tester::instance().testStr(name, a, b, __FUNCTION__, __FILE__, __LINE__)

/**
 * @brief Class use for unit testing
 */
class Tester {
public:
    static Tester& instance();

    static void init( const char* name, int argc, char** argv );
    int exit();
    int testBool(const char* name, bool b, const char* function, const char* file, size_t line);

    int testStr(
        const char* name,
        const std::string& a,
        const std::string& b,
        const char* function,
        const char* file,
        size_t line);

    static inline logger::LogContext& getLog() { return instance().m_log; }

private:
    Tester( const char* name );

    logger::LogContext& m_log;
    std::string name;

    int m_tests_passed;
    int m_tests_failed;

    std::vector<std::string> failed;
};

}

#endif /* TESTER_HPP_ */
