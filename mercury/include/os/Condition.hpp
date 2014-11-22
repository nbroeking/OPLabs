#ifndef CONDITION_HPP_
#define CONDITION_HPP_

#include <pthread.h>
#include <os/Time.hpp>
#include <os/Mutex.hpp>

namespace os { 
/**
 * @brief Class defining a condition variable from the pthreads library
 * @author jrahm
 *
 * Then there is the thing about the doc comments
 * And this is just a description
 */
class Condition {
public:
    /*
     * Default constructor. Initializes the condition.
     */
    Condition() ;

    /**
     * @brief Signal this condition and to wake up the thread listening
     * on it.
     */
    virtual void signal() ;

    /**
     *
     * @brief wait on a condition. A mutex must be passed in to prevent a race
     *        condition.
     *
     * @param mutex the mutex to pass in
     *
     * The point of the mutex in this function is to avoid the race condition
     * where the signal comes right before the wait, causing deadlock. The
     * mutex must be locked when passed to this function. This function will
     * atomically unlock the mutex and wait on the condition.
     */
    virtual bool wait( Mutex& mutex ) ;

    /**
     * @brief Like wait, but with a timeout.
     *
     * @see wait(Mutex&)
     * @param mutex the guarding mutex
     * @param timeout the timeout to wait for a signal
     *
     * @return true if the signal was received, false if the timeout was hit
     *
     * Waits for the condition to be signaled for `timeout` microseconds.
     */
	virtual bool timedwait( Mutex& mutex, timeout_t timeout );

    virtual ~Condition() ;
private:
    pthread_cond_t m_condition ;
} ;

}

#endif /* CONDITION_HPP_ */
