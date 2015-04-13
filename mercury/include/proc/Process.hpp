#ifndef PROCESS_HPP_
#define PROCESS_HPP_

/*
 * Author: jrahm
 * created: 2015/01/20
 * Process.hpp: <description>
 */

#include <io/FileCollection.hpp>
#include <io/Buffer.hpp>
#include <os/Thread.hpp>
#include <os/Scheduler.hpp>
#include <containers/BlockingQueue.hpp>

namespace proc {

class _InternalThread;

/**
 * A process is a type of Thread that has some extra
 * support for asyncronicity.
 *
 * A process is an entity that is global on a per thread
 * basis.
 */
class Process: public os::ManagedRunnable {
public:
    Process(const char* name);

    /**
     * return the name of this process
     *
     * @return the name of this process
     */
    inline const char* getName() {
        return name.c_str();
    }

    /**
     * Gets the controlling process of the currently executing
     * thread. May return NULL if the thread has no controlling
     * process
     */
    static Process& getProcess();

    /**
     * Return the main FileCollection used to handle
     * main input and output.
     */
    io::FileCollection& getFileCollection();

    /**
     * @brief Create a new thread under this process
     * @param runner The runnable to run
     * @return A new thread running under this process
     *
     * This should be the preferred method of creating a
     * new process when operating under the process
     * model.
     */
    os::Thread* newThread(os::ManagedRunnable& runner);

    /* the run method. Starts executing a process */
    virtual void run() = 0;

    inline logger::LogContext* getLogContext() {
        return m_log;
    }

    inline os::Scheduler& getScheduler() {
        return m_scheduler;
    }

    /**
     * @brief Starts this process in a new thread
     * @return The new thread created by the process
     */
    virtual os::Thread* start();
    virtual inline os::Thread* getSchedulingThread() {
        return m_sched;
    }

    virtual void stop();
    virtual void join();

private:
    io::FileCollection m_file_collection;
    std::string name;
    os::Scheduler m_scheduler;

    std::vector<_InternalThread*> m_threads;
    bool stopping;
    os::Thread* m_thread;

    os::Thread* m_sched;

protected:
    logger::LogContext* m_log;
};

/* A process that starts with a mutex locked
 * to prevent race conditions */
class ProtectedProcess: public Process {
public:
    ProtectedProcess(const char* name): Process(name) {}
    virtual os::Thread* start() {
        m_mutex.lock();
        return Process::start();
    }

protected:
    os::Mutex m_mutex;
};

}

#endif /* PROCESS_HPP_ */
