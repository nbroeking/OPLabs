#ifndef SCHEDULER_HPP_
#define SCHEDULER_HPP_

/*
 * Author: jrahm
 * created: 2015/02/23
 * Scheduler.hpp: <description>
 */

#include <os/ScopedLock.hpp>
#include <os/Thread.hpp>
#include <os/Time.hpp>
#include <os/Condition.hpp>
#include <containers/PriorityQueue.hpp>
#include <containers/BlockingQueue.hpp>

#include <log/LogManager.hpp>

namespace os {

class Scheduler: public Runnable {
public:
    Scheduler();

    void schedule(Runnable* r, timeout_t when) ;

    void cancel(Runnable* r) ;

    void stop() ;

    void setStopOnEmpty(bool stop) ;

    void run() ;

    inline bool verify_queue() {
        return m_queue.verify();
    }

private:
    class Schedule {
    public:
        Runnable* to_run;
        timeout_t when;
        bool operator==(const Schedule& oth) { return oth.to_run == to_run; }
    };

    class ScheduleComp {
    public:
        inline bool operator()(const Schedule& s1, const Schedule& s2) {
            return s1.when < s2.when;
        }
    };

    enum RequestType {
        EXIT,
        SCHEDULE,
        CANCEL,
        CONTINUE
    };

    struct Request {
        RequestType typ;

        union {
            struct {
                Schedule sched;
            } sched;
            struct {
                Runnable* runner;
            } cancel;
        } un;
    };

    logger::LogContext* m_log;

    containers::BlockingQueue<Request> m_request_queue;
    containers::PriorityQueue<Schedule, ScheduleComp> m_queue;

    bool m_empty_stop;

};

};

#endif /* SCHEDULER_HPP_ */
