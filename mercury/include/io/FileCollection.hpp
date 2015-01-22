#ifndef POLLGROUP_HPP_
#define POLLGROUP_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * PollGroup.hpp: <description>
 */

#include <io/HasRawFd.hpp>

#include <prelude.hpp>
#include <map>
#include <vector>
#include <log/LogManager.hpp>
#include <os/Runnable.hpp>

namespace io {

/**
 * An observer for a file collection which will be called when there is
 * an event on a file descriptor.
 */
class FileCollectionObserver {
public:
    /**
     * @brief The method called when there is an event
     * @param fd The file descriptor that had the event
     * @param events The bitmask of events that has occured
     */
    virtual void observe(int fd, int events) = 0;
};

/**
 * A FileCollectionObserver that has a HasRawFd object
 * directly associated with it.
 */
class SingleFileCollectionObserver : public FileCollectionObserver {
public:
    /**
     * @brief Return the file descriptor associated with  
     * @return <what>
     */
    virtual HasRawFd* getFd() = 0;
};

/**
 * @brief A collection of file descriptors that can be run as a thread.
 */
class FileCollection: public os::Runnable {
public:
    FileCollection();
    
    /**
     * @brief Subscribe an observer to the file descriptor and wait for data to read.
     * @param fd The file descriptor to read from
     * @param listener The listener to be called
     */
    void subscribeForRead(int fd, FileCollectionObserver* listener);
    void subscribeForRead(HasRawFd* fd, FileCollectionObserver* listener) {
        subscribeForRead(fd->getRawFd(), listener);
    }

    /**
     * @brief Like the above, ubt uses the getFd function to get the file descriptor
     * @param listener the listener to callback
     */
    void subscribeForRead(SingleFileCollectionObserver* listener) {
        subscribeForRead(listener->getFd()->getRawFd(), listener);
    }

    /**
     * @brief Subscribe a FileCollectionObserver for notification when fd is available for write
     * @param fd The file descriptor available for events
     * @param listener The listener to attach.
     */
    void subscribeForWrite(int fd, FileCollectionObserver* listener);

    /**
     * @brief Begin a loop that polls and waito for an event.
     */
    void run();

    ~FileCollection();
protected:
    void interrupt();

private:
    /* The log context for FileCollection */
    logger::LogContext* m_log;
    int m_pipe[2]; 
    void fireEvent( int fd, int events );

    std::map<int, std::vector<FileCollectionObserver*> > m_map;
    std::map<int, std::vector<FileCollectionObserver*> > m_write_map;
};

}

#endif /* POLLGROUP_HPP_ */
