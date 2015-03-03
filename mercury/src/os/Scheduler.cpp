#include <os/Scheduler.hpp>

namespace os {

Scheduler::Scheduler():
    m_mutex(),
    drop(), del(false), exit(false), m_empty_stop(false) {
        m_log = &logger::LogManager::instance().getLogContext("os", "Scheduler");
    }

void Scheduler::schedule(Runnable* r, timeout_t when) {
    m_guard.lock();
    ScopedLock __sl(m_mutex);
    m_log->printfln(TRACE, "Request to schedule %p\n", r);
    drop.to_run = r;
    drop.when = Time::relativeToAbsoluteTime(when);
    cont = false;
    del = false;
    cond.signal();
}

void Scheduler::cancel(Runnable* r) {
    m_guard.lock();
    ScopedLock __sl(m_mutex);
    drop.to_run = r;
    cont = false;
    del = true;
    cond.signal();
}

void Scheduler::stop() {
    m_guard.lock();
    ScopedLock __sl(m_mutex);
    cont = false;
    exit = true;
    cond.signal();
}

void Scheduler::setStopOnEmpty(bool stop) {
    ScopedLock __sl(m_mutex);
    m_empty_stop = stop;
    cont = true;
    cond.signal();
}

Thread* Scheduler::start() {
    m_mutex.lock();
    Thread* ret = Thread::begin(* this);
    return ret;
}

void Scheduler::run() {
    while ( !(m_empty_stop && m_queue.empty()) ) {
        bool rc = true;
        if( m_queue.empty() ) {
            m_log->printfln(DEBUG, "Queue is empty, waiting for signal");
            cond.wait( m_mutex );
        } else {
            Schedule& top = m_queue.front();
            timeout_t wait = Time::absoluteTimeToRelativeTime(top.when);
            wait = wait < 0 ? 0 : wait;
            m_log->printfln(DEBUG, "Waiting for %f seconds", wait / 1000000.0);
            rc = cond.timedwait(m_mutex, wait);

        }

        if( rc ) {
            m_log->printfln(DEBUG, "Signal received");

            if ( cont ) {
                continue;
            } else if ( exit ) {
                m_log->printfln(INFO, "Exiting");
                m_guard.unlock();
                return ;
            } else if ( del ) {
                m_log->printfln(INFO, "Removing Runnable %p", drop.to_run);
                m_queue.remove( drop );
            } else {
                m_log->printfln(INFO, "Adding Runnable %p", drop.to_run);
                m_queue.push( drop );
            }

            m_guard.unlock();
        } else {
            m_queue.front().to_run->run();
            m_queue.pop();
        }
    }
}

}
