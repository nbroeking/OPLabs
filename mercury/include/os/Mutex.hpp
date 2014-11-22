#ifndef MUTEX_HPP_
#define MUTEX_HPP_

/*
 * Author: jrahm
 * created: 2014/11/11
 * Mutex.hpp: <description>
 */

#include <os/Thread.hpp>
#include <os/Time.hpp>

namespace os {

/**
 * @brief class defining a mutex from the pthread library
 * @author jrahm
 *
 * The mutex acts as a simple lock around critical sections.
 */
class Mutex {
public:
	Mutex( );

    /**
     * @brief Lock the mutex.
     */
	int lock();

    /**
     * @brief Unlock the mutex
     */
	int unlock();

    /**
     * @brief attempt to lock the mutex
     * @return 0 if the mutex was locked, 1 otherwise
     * @see pthread_mutex_trylock
     *
     * This function does not block
     */
	int try_lock();

    /**
     * @brief timed version of lock()
     * @return 0 if successful otherwise a non-zero is returned indicating the error
     * @see pthread_mutex_timedlock
     */
	int timedlock( timeout_t timeout );

    /**
     * @brief The raw underlying wrapper to the POSIX mutex
     */
	inline pthread_mutex_t* raw() { return &m_mutex; };
private:
	pthread_mutex_t m_mutex;
};

}

#endif /* MUTEX_HPP_ */
