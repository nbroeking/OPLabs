#ifndef TIME_HPP_
#define TIME_HPP_

#include <time.h>
#include <types.h>

/*
 * Author: jrahm
 * created: 2014/11/10
 * Time.hpp: <description>
 */

namespace os {

typedef int64_t timeout_t ;

class Time {
public:
	/* 
	 * Populate a timespec structure with the value equal
	 * to now + number of micro seconds it the future 
	 */
	static void microsInFuture( struct timespec* ts, timeout_t micros );

	/*
	 * Populate a timespec struct with the absolute value
	 * of microseconds since EPOCH
	 */
	static void fromMicros( struct timespec&, timeout_t micros );

	/*
	 * Convert a timespec to microseconds
	 */
	static timeout_t toMicros( struct timespec& ts ); 

	/*
	 * Sleep for a certain number of microseconds
	 */
	static timeout_t sleep( timeout_t );
};

}

#endif /* TIME_HPP_ */
