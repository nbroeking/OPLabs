
#include <sys/poll.h>
#include <unistd.h>

#define HAS_FIONREAD

#include <sys/stat.h>

#ifdef HAS_FIONREAD
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

#include <io/FileCollection.hpp>

using namespace std;
using namespace logger;
using namespace lang;
using namespace os;


namespace io {

const char* subscribe_str[] = {
    "ServerSocket", "Read", "Write"
};


FileCollection::FileCollection():
    m_log(LogManager::instance().getLogContext("IO", "FileCollection::Static")) {
    pipe( m_pipe );
    this->subscribe(SUBSCRIBE_READ, m_pipe[0], NULL);
}

void FileCollection::interrupt() {
    m_log.printfln(DEBUG, "Interrupt called");
    byte bytes[1];
	bytes[0] = 0;
    write(m_pipe[1], bytes, 1);
}

void FileCollection::subscribe( SubscriptionType type, int f,
    FileCollectionObserver* observer, dealloc_T* dealloc) {
    if( type > SUBSCRIBE_N ) {
        throw InvalidArgumentException("Enum out of bounds");
    }

    ScopedLock __sl(m_mutex);
    m_log.printfln(DEBUG, "Subscribing new file descripotr %d with observer %p for %s",
        f, observer, subscribe_str[type]
    );

    entry_T entry = make_pair( type, observer );
    m_deallocators[observer] = dealloc;
    m_map[f] = entry;

    interrupt();
}

void FileCollection::fireEvent( int fd, int events ) {
    m_log.printfln(DEBUG, "FireEvent for file descriptor %d events=%d,%d,%d|%d,%d,%d", fd,
        !!(events&POLLIN),
        !!(events&POLLOUT),
        !!(events&POLLPRI),

        !!(events&POLLERR),
        !!(events&POLLHUP),
        !!(events&POLLNVAL)
    );

    FileCollectionObserver* obs = m_map[fd].second;
    if( obs ) {
        obs->observe(fd, events);
    }
}

int subscriptionTypeToBitmask( FileCollection::SubscriptionType typ ) {
    switch(typ) {
    case FileCollection::SERVER_SOCKET:
    case FileCollection::SUBSCRIBE_READ:
        return POLLIN;
    case FileCollection::SUBSCRIBE_WRITE:
        return POLLOUT;
    default:
        throw InvalidArgumentException("Enum out of range");
    }

}


void FileCollection::handle_poll_results( 
    logger::LogContext& log,
    std::vector<struct pollfd>& poll_data ) {

    vector<int> unsubscriptions;
    vector< pair<int,int> > events;
    m_mutex.lock();

    log.printfln(DEBUG, "Events detected");
    vector<struct pollfd>::iterator vitr;

    for( vitr = poll_data.begin(); vitr != poll_data.end(); ++ vitr ) {
        if( vitr->revents != 0 ) {

            entry_T entry = m_map[vitr->fd];
            if( vitr->fd == m_pipe[0] ) {
                /* There was an interrupt, throw away the
                 * data */
                log.printfln(DEBUG, "There is an interrupt");
                byte bytes[1024];
                read(m_pipe[0], bytes, 1024);
            } else {
                /* Fire the event for the correct file descriptor */
                if( vitr->revents & POLLHUP ) {

                    /* There was a hangup, we need to remove the
                     * file descriptor */
                    log.printfln(DEBUG, "Unsubscribing hungup file descriptor %d", vitr->fd);
                    unsubscriptions.push_back(vitr->fd);
                } else {
                    /* There _might_ be data to read */
#ifdef  HAS_FIONREAD
                    bool not_is_server = entry.first != SERVER_SOCKET;
                    long n_read = 1;
                    int err = 0;

                    if( not_is_server && (err = ioctl( vitr->fd, FIONREAD, &n_read )) ){
                        log.printfln(WARN, "ioctl failed: %s", strerror(err));
                    }

                    if( n_read > 0 ) {
#endif
                        /* This file descriptor had an event.
                         * Fire that event */
                        events.push_back( make_pair(vitr->fd, vitr->revents) );

#ifdef  HAS_FIONREAD
                    } else {
                        log.printfln(DEBUG, "Unsubscrbing %d due to EOF", vitr->fd);
                        unsubscriptions.push_back(vitr->fd);
                    }
#endif
                }
            }

        }
    }

    m_mutex.unlock();
    vector<int>::iterator itr1;
    vector< pair<int,int> >::iterator itr2;
    
    FOR_EACH(itr1, unsubscriptions) {
        _unsubscribe( *itr1 );
    }

    FOR_EACH(itr2, events) {
        fireEvent(itr2->first, itr2->second);
    }

}

void FileCollection::run() {
    LogContext& log = LogManager::instance().getLogContext("IO", "FileCollection::Run");
    log.printfln(DEBUG, "Start FileCollection::run");

    for( ; ; ) {
        vector<struct pollfd> poll_data;
        map_T::iterator itr;

        /* Add the poll data from the reading map */
        for( itr = m_map.begin() ; itr != m_map.end() ; ++ itr ) {
            log.printfln(TRACE,"Adding for input set fd %d", itr->first);

            struct pollfd pfd;
            pfd.revents = 0;
            pfd.fd = itr->first;
            pfd.events = subscriptionTypeToBitmask(itr->second.first);
            poll_data.push_back(pfd);
        }

        int rc = poll(&poll_data[0], poll_data.size(), 5000);

        if( rc == -1 ) {
            throw CException("Error in poll", rc);
        }

        if( rc != 0 ) {
            handle_poll_results( log, poll_data );
        }

    }
}

bool FileCollection::_unsubscribe( int fd ) {
    m_log.printfln(DEBUG, "Unsubscribing file descriptor %d", fd);

    map_T::iterator itr = m_map.find(fd);

    if( itr == m_map.end() ) return false;

    if( itr->second.second )
        itr->second.second->onUnsubscribe(fd);

    dealloc_map_T::iterator ditr;
    ditr = m_deallocators.find( itr->second.second );

    if( ditr != m_deallocators.end() ) {
        if( ditr->second ) {
            ditr->second->deallocate( itr->second.second );
        }
        m_deallocators.erase(ditr);
    }

    m_map.erase(itr);

    return true;
}

bool FileCollection::unsubscribe( int fd ) {
    ScopedLock __sl( m_mutex );
    interrupt();
    bool ret = _unsubscribe(fd);
    return ret;
}

FileCollection::~FileCollection() {
    close(m_pipe[0]);
    close(m_pipe[1]);
}

}
