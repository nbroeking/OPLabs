#include <os/Scheduler.hpp>

namespace os {

Scheduler::Scheduler() {
    m_log = &logger::LogManager::instance().getLogContext("os", "Scheduler");
    m_empty_stop = false;
}

void Scheduler::schedule(Runnable* r, timeout_t when) {
    m_log->printfln(TRACE, "Schedule timeout in %d us for Runnable %p", when, r);

    Request req;
    req.typ = SCHEDULE;
    req.un.sched.sched.to_run = r; req.un.sched.sched.when = when;
    m_request_queue.push(req);
}

void Scheduler::cancel(Runnable* r) {
    m_log->printfln(TRACE, "Cancel runnable", r);

    Request req;
    req.typ = CANCEL;
    req.un.cancel.runner = r;
    m_request_queue.push(req);
}

void Scheduler::stop() {
    Request req;
    req.typ = EXIT;
    m_request_queue.push(req);
}

void Scheduler::setStopOnEmpty(bool stop) {
    m_empty_stop = stop;
    Request req;
    req.typ = CONTINUE;
    m_request_queue.push(req);
}

void Scheduler::run() {
    Request request;
    while ( !(m_empty_stop && m_queue.empty()) ) {
        bool rc = true;
        if( m_queue.empty() ) {
            m_log->printfln(TRACE, "Queue is empty, waiting for request");
            request = m_request_queue.front();
            m_log->printfln(TRACE, "wait over");
        } else {
            Schedule& top = m_queue.front();
            timeout_t wait = Time::absoluteTimeToRelativeTime(top.when);
            wait = wait < 0 ? 0 : wait;

            m_log->printfln(TRACE, "Waiting for %f seconds", wait / 1000000.0);
            rc = m_request_queue.front_timed(request, wait);
            m_log->printfln(TRACE, "wait over");

        }

        if( rc ) {
            /* There was no timeout, this implies a request */
            m_log->printfln(TRACE, "Signal received");
            m_request_queue.pop();

            switch(request.typ) {
            case CONTINUE:
                break; 

            case EXIT:
                m_log->printfln(DEBUG, "Exiting");
                return;
                break;

            case CANCEL:
                m_log->printfln(DEBUG, "Removing Runnable %p", request.un.cancel.runner);
                Schedule sched;
                sched.to_run = request.un.cancel.runner;
                m_queue.remove(sched);
                break;

            case SCHEDULE:
                m_log->printfln(DEBUG, "Scheduling Runnable %p for %f seconds",
                    request.un.sched.sched.to_run,
                    request.un.sched.sched.when / 1000000.0);
                request.un.sched.sched.when = Time::relativeToAbsoluteTime(request.un.sched.sched.when);
                m_queue.push(request.un.sched.sched);
                break;

            }
        } else {
            m_log->printfln(TRACE, "Executing runnable %p", m_queue.front().to_run);
            m_queue.front().to_run->run();
            m_queue.pop();
        }
    }
}

}
