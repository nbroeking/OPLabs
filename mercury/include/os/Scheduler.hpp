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

/**
 * @brief A class that schedules things to run at a 
 * time in the future
 */
class Scheduler: public ManagedRunnable {
public:
    Scheduler();

    /**
     * @brief Schedule a runnable to run at `when` microseconds in the future
     * 
     * @param r The runnable to run
     * @param when Number of mircos in the future to run
     */
    void schedule(Runnable* r, timeout_t when) ;

    /**
     * @brief Stop a runnable from running
     * @param r The runnable to cancel
     */
    void cancel(Runnable* r) ;

    /**
     * @brief Stop the scheduler. Cancels all scheduled jobs and returns
     */
    void stop() ;

    /**
     * @brief Set whether this scheduler should stop once all jobs have completed
     * @param stop True if the scheduler should stop, false otherwise
     */
    void setStopOnEmpty(bool stop) ;

    /**
     * @brief Scheduler main loop
     */
    void run() ;

    /* For debugging */
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
