#include <os/Thread.hpp>
#include <map>

namespace os {

std::map<pthread_t, Thread*> s_thread_db;

Thread* Thread::getCurrentThread() {
    return s_thread_db[pthread_self()];
}

static void* run_thread( void* _thread ) {
	Thread* thread = (Thread*) _thread;
	thread->getRunnable().run();

	pthread_exit(NULL);
}

Thread::Thread(Runnable& runner) : m_runner(runner) {
	m_thread = -1;
}

int Thread::start() {
    int rc;
	rc = pthread_create(&m_thread, NULL, run_thread, this);
    s_thread_db[m_thread] = this;
    return rc;
}

int Thread::join() {
	return pthread_join(m_thread, NULL);
}

Thread::~Thread() {
    s_thread_db[m_thread]=NULL;
}

}
