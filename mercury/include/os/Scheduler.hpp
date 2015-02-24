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
#include <log/LogManager.hpp>

namespace os {

class Scheduler: Runnable {
public:
    Scheduler();

    void schedule(Runnable* r, timeout_t when) ;

    void cancel(Runnable* r) ;

    void stop() ;

    void setStopOnEmpty(bool stop) ;

    Thread* start() ;

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



    Mutex m_mutex;
    Mutex m_guard;

    Condition cond;

    Schedule drop;
    bool del;
    bool exit;
    bool m_empty_stop;
    bool cont;

    logger::LogContext* m_log;
    containers::PriorityQueue<Schedule, ScheduleComp> m_queue;

};

};

#endif /* SCHEDULER_HPP_ */
