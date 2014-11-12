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

class Mutex {
public:
	Mutex( );

	int lock();
	int unlock();
	int try_lock();

	int timedlock( timeout_t timeout );

	inline pthread_mutex_t* raw() { return &m_mutex; };
private:
	pthread_mutex_t m_mutex;
};

}

#endif /* MUTEX_HPP_ */
