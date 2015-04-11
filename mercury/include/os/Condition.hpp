#ifndef CONDITION_HPP_
#define CONDITION_HPP_

#include <pthread.h>
#include <os/Time.hpp>
#include <os/Mutex.hpp>

namespace os { 

class ConditionException: public CException {
public:
    ConditionException(const char* message, int rc):
        CException(message, rc){}
};

/**
 * @brief Class defining a condition variable from the pthreads library
 * @author jrahm
 *
 * Then there is the thing about the doc comments
 * And this is just a description
 *
 * @code
 * int bocking_take(Mutex& m, Condition& cond, queue<int>& queue) {
 *  m.lock();
 *  // begin critical section
 *  if ( queue.empty() )
 *      // wait for the signal
 *      cond.wait(m);
 *  value = queue.front();
 *  m.unlock();
 *  // end critical section
 *  return value;
 * }
 *
 * void blocking_push(Mutex& m, Condition& cond, queue<int>& queue) {
 *  m.lock();
 *  queue.push(5);
 *  cond.signal();
 *  // blocking_take will now continue once the mutex is free
 *  m.unlock();
 * }
 * @endcode
 *
 * @see Mutex
 * @see ScopedLock
 * @see Thread
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
