#ifndef TIME_HPP_
#define TIME_HPP_

#include <Prelude>

#if defined(TARGET_GROUP_mips) || defined(__OpenBSD__)
#include <sys/time.h>
#endif

#include <time.h>

/*
 * Author: jrahm
 * created: 2014/11/10
 * Time.hpp: <description>
 */

namespace os {

typedef int64_t timeout_t ;
typedef timeout_t micros_t ;

#define HOURS   * (60 MINUTES)
#define MINUTES * (60 SECS)
#define SECS    * (1000 MILLIS)
#define MILLIS  * (1000 MICROS)
#define MICROS  * 1

class TimeoutException: public Exception {
public:
	TimeoutException() : Exception("Timeout") {}
	TimeoutException(const char* n) : Exception(n) {}
};

/**
 * @brief Class for easier time handling
 */
class Time {
public:
    /**
     * @brief Populate a timespec structure with the value equal to now + micros
     * 
     * @param ts the structure to populate
     * @param micros the number of micro seconds in the future
     */
	static void microsInFuture( struct timespec* ts, timeout_t micros );

    /**
     * @brief Convert a timespec to micro seconds
     * 
     * @param ts The timespec to use
     * @param micros A reference to the microseconds
     */
	static void fromMicros( struct timespec& ts, timeout_t micros );

    /**
     * @brief Like tho above, but doesn't use a reference
     * @param ts The timespec to convert
     * @return ts in the form of microseconds
     */
	static timeout_t toMicros( struct timespec& ts ); 

	/**
	 * Sleep for a certain number of microseconds
     * @param to the timeout to sleep for
     * @return the amount of time left 
	 */
	static timeout_t sleep( timeout_t to );

    static inline struct timeval toTimeval( micros_t mics ) {
        struct timeval tv;
        tv.tv_sec = mics / 1000000;
        tv.tv_usec = mics % 1000000;
        return tv;
    }
};

}

#endif /* TIME_HPP_ */
