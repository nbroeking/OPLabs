#include <pthread.h>
#include <os/Runnable.hpp>

namespace os {

/* 
 * A simple thread class that accepts a runnable
 * as a delegate. This class should not be extendend.
 * instead such things should be done through the runnable
 * delegate
 */
class Thread {
public:
	/* Create a new thread that runs the specified
	 * runnable */
    Thread(Runnable& runner) ;

	/* Start the thread */
	int start();

	/* Join the thread */
	int join();

	inline Runnable& getRunnable() { return m_runner; }
	inline const Runnable& getRunnable() const { return m_runner; }
private:
	Runnable& m_runner;
	pthread_t m_thread;
};

}
