#include <os/Time.hpp>
#include <cstdlib>
#ifdef __MACH__
#include <mach/mach.h>
#include <mach/clock.h>
#include <mach/mach_error.h>
#endif

namespace os {

static const u64_t billion = 1000000000;

void Time::microsInFuture( struct timespec* ts, timeout_t micros ) {
	#ifdef __MACH__
		clock_serv_t cclock;
		mach_timespec_t mts;
		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
		clock_get_time(cclock, &mts);
		mach_port_deallocate(mach_task_self(), cclock);
		ts->tv_sec = mts.tv_sec;
		ts->tv_nsec = mts.tv_nsec;
	#else
		clock_gettime(CLOCK_REALTIME, ts);
	#endif


    u64_t nsec = micros * 1000;
    u64_t nsec_mod = nsec % billion;

	ts->tv_sec += nsec / billion;
	ts->tv_nsec = nsec_mod;
}

void Time::fromMicros( struct timespec& ts, timeout_t timeout ) {
    u64_t nsec = timeout * 1000;
    u64_t nsec_mod = nsec % billion;
    u64_t sec = nsec / billion;

	ts.tv_sec = sec ;
	ts.tv_nsec = nsec_mod ;
}

timeout_t Time::toMicros( struct timespec& ts ) {
	return (ts.tv_sec * 1000000000 + ts.tv_nsec) / 1000 ;
}

timeout_t Time::sleep( timeout_t timeout ) {
	struct timespec ts ;
	struct timespec ts_out ;
	Time::fromMicros( ts, timeout ) ;
	nanosleep( &ts, &ts_out ) ;
	return toMicros( ts_out ) ;
}

timeout_t Time::relativeToAbsoluteTime( timeout_t relative ) {
    return relative + currentTimeMicros();
}

timeout_t Time::absoluteTimeToRelativeTime( timeout_t absolute ) {
    return absolute - currentTimeMicros();
}

timeout_t Time::currentTimeMicros() {
	#ifdef __MACH__
		clock_serv_t cclock;
		mach_timespec_t mts;
		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
		clock_get_time(cclock, &mts);
		mach_port_deallocate(mach_task_self(), cclock);
	#else
        struct timespec mts;
		clock_gettime(CLOCK_REALTIME, &mts);
	#endif

    return mts.tv_nsec/ 1000 + mts.tv_sec * 1000000;
}

}
