#include <os/Mutex.hpp>

namespace os {

Mutex::Mutex() {
	pthread_mutex_init( &m_mutex, NULL );
}

int Mutex::lock() {
	return pthread_mutex_lock( &m_mutex );
}

int Mutex::unlock() {
	return pthread_mutex_unlock( &m_mutex );
}

int Mutex::try_lock() {
	return pthread_mutex_trylock( &m_mutex );
}

int Mutex::timedlock( timeout_t timeout ) {
#ifdef __linux__
	struct timespec ts;
	Time::microsInFuture( &ts, timeout );
	return pthread_mutex_timedlock(&m_mutex, &ts);
#else
	/* ummm ... TODO: I don't have a good solution */
	return lock();
#endif
}

}
