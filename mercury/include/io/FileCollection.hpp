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

    /**
     * @brief called when the file descriptor is closed
     * @param fd the closed file descriptor
     */
    virtual void onUnsubscribe(int fd) { (void) fd; };

    inline virtual ~FileCollectionObserver(){};
};


/**
 * @brief A collection of file descriptors that can be run as a thread.
 */
class FileCollection: public os::Runnable {
public:
    FileCollection();

    enum SubscriptionType {
        SERVER_SOCKET,
        SUBSCRIBE_READ,
        SUBSCRIBE_WRITE,
        SUBSCRIBE_N
    };
    
    /**
     * @brief Subscribe a file descriptor to this socket collection under
     *          the given subscription type.
     * 
     * @param typ the type of subscription
     * @param fd the file descriptor to listen to
     * @param observer the observer to listen to
     * @param dealloc the deallocator to use for the observer
     */
    void subscribe( SubscriptionType typ,
                    int fd,
                    FileCollectionObserver* observer, 
                    lang::Deallocator<FileCollectionObserver>* dealloc = NULL);

    /**
     * @brief Similar to the above, but with the convinience of autounboxing the HasRawFd
     * 
     * @param typ the type of subscription
     * @param fd the object to subscribe to this file collection
     * @param observer the observer to be triggered when there is an event
     * @param dealloc the deallocator for the observer
     */
    inline void subscribe( SubscriptionType typ, HasRawFd* fd,
                           FileCollectionObserver* observer,
                           lang::Deallocator<FileCollectionObserver>* dealloc = NULL ){
        subscribe(typ, fd->getRawFd(), observer, dealloc);
    }

    /**
     * @brief unsubscribe from a socket collection
     * @param fd the file descriptor to unsubscribe
     * @return true if the file descriptor was found and unsubscribed
     */
    bool unsubscribe( int fd );
    inline bool unsubscribe( HasRawFd* fd ) { return unsubscribe(fd->getRawFd()); };


    /**
     * @brief Begin a loop that polls and waito for an event.
     */
    void run();

    ~FileCollection();

protected:
    void interrupt();

private:
    typedef lang::Deallocator<FileCollectionObserver> dealloc_T;
    typedef std::pair<SubscriptionType, FileCollectionObserver*> entry_T;

    typedef std::map<int, entry_T> map_T;
    typedef std::map<FileCollectionObserver*, dealloc_T*> dealloc_map_T;

    /* The log context for FileCollection */
    logger::LogContext& m_log;

    /* the pipe is a way to specify */
    int m_pipe[2]; 

    /* file an event */
    void fireEvent( int fd, int events );
    void handle_poll_results(
        logger::LogContext& log,
        std::vector<struct pollfd>& poll_data );
    bool _unsubscribe( int fd );

    /* map from observers to their deallocators */
    dealloc_map_T m_deallocators;

    /* map from file descriptors to (type, observers) */
    map_T m_map;

    os::Mutex m_mutex;
};

}

#endif /* POLLGROUP_HPP_ */
