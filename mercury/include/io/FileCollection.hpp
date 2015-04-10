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

#include <lang/Deallocator.hpp>
#include <io/ServerSocket.hpp>

#include <containers/BlockingQueue.hpp>

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
    virtual void observe(HasRawFd* fd, int events) = 0;

    /**
     * @brief called when the file descriptor is closed
     * @param fd the closed file descriptor
     */
    virtual void onUnsubscribe(HasRawFd* fd) { (void) fd; };

    inline virtual ~FileCollectionObserver(){};
};


/**
 * @brief A collection of file descriptors that can be run as a thread.
 */
class FileCollection: public os::ManagedRunnable {
public:
    FileCollection();

    enum SubscriptionType {
        SERVER_SOCKET,
        SUBSCRIBE_READ,
        SUBSCRIBE_WRITE,
        SUBSCRIBE_N
    };
    
    /**
     * @brief Similar to the above, but with the convinience of autounboxing the HasRawFd
     * 
     * @param typ the type of subscription
     * @param fd the object to subscribe to this file collection
     * @param observer the observer to be triggered when there is an event
     * @param dealloc the deallocator for the observer
     */
    void subscribe( SubscriptionType typ, HasRawFd* fd,
                           FileCollectionObserver* observer,
                           lang::Deallocator<FileCollectionObserver>* dealloc = NULL );

    /**
     * @brief unsubscribe from a socket collection
     * @param fd the file descriptor to unsubscribe
     * @return true if the file descriptor was found and unsubscribed
     */
    bool unsubscribe( HasRawFd* fd ) ;


    /**
     * @brief Begin a loop that polls and waito for an event.
     */
    void run();

    void stop();

    ~FileCollection();

protected:
    void interrupt();

private:
    class Poller;
    friend class Poller;
    struct Command;

    Poller* m_poller;

    /* The log context for FileCollection */
    logger::LogContext& m_log;
};

}

#endif /* POLLGROUP_HPP_ */
