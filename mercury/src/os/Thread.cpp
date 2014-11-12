#include <os/Thread.hpp>

namespace os {

static void* run_thread( void* _thread ) {
	Thread* thread = (Thread*) _thread;
	thread->getRunnable().run();

	pthread_exit(NULL);
}

Thread::Thread(Runnable& runner) : m_runner(runner) {
	m_thread = -1;
}

int Thread::start() {
	return pthread_create(&m_thread, NULL, run_thread, this);
}

int Thread::join() {
	return pthread_join(m_thread, NULL);
}

}
