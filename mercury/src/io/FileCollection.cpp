#include <io/FileCollection.hpp>

#include <sys/poll.h>
#include <unistd.h>

using namespace std;
using namespace logger;
using namespace lang;

namespace io {


FileCollection::FileCollection() {
    pipe( m_pipe );
    m_map[m_pipe[0]] = NULL;
    m_log = &LogManager::instance().getLogContext("IO", "FileCollection::Static");
}

void FileCollection::interrupt() {
    m_log->printfln(DEBUG, "Interrupt called");
    byte bytes[1];
	bytes[0] = 0;
    write(m_pipe[1], bytes, 1);
}

void FileCollection::subscribeForRead( int fd, FileCollectionObserver* observer,
    Deallocator<FileCollectionObserver>* dealloc ) {
    m_log->printfln(DEBUG, "Subscribing new file descriptor %d with observer %p for read", fd, observer);

    m_map[fd] = observer;
    if ( dealloc ) {
        m_deallocators[observer] = dealloc;
    }

    interrupt();
}

void FileCollection::subscribeForWrite( int fd, FileCollectionObserver* observer,
       dealloc_T* dealloc ) {

    m_log->printfln(DEBUG, "Subscribing new file descriptor %d with observer %p for write", fd, observer);
    m_map[fd] = observer;
    if( dealloc ) {
        m_deallocators[observer] = dealloc;
    }
    interrupt();
}

void FileCollection::fireEvent( int fd, int events ) {
    m_log->printfln(DEBUG, "FireEvent for file descriptor %d events=%d,%d,%d|%d,%d,%d", fd,
        !!(events&POLLIN),
        !!(events&POLLOUT),
        !!(events&POLLPRI),

        !!(events&POLLERR),
        !!(events&POLLHUP),
        !!(events&POLLNVAL)
    );

    FileCollectionObserver* obs = m_map[fd];
    if( obs ) {
        obs->observe(fd, events);
    }
}

void FileCollection::run() {
    LogContext& log = LogManager::instance().getLogContext("IO", "FileCollection::Run");
    log.printfln(DEBUG, "Start FileCollection::run");

    for( ; ; ) {
        vector<struct pollfd> poll_data;
        map<int, FileCollectionObserver* >::iterator itr;

        /* Add the poll data from the reading map */
        for( itr = m_map.begin() ; itr != m_map.end() ; ++ itr ) {
            log.printfln(TRACE,"Adding for input set fd %d", itr->first);

            struct pollfd pfd;
            pfd.revents = 0;
            pfd.fd = itr->first;
            pfd.events = POLLIN | POLLPRI;
            poll_data.push_back(pfd);
        }

        /* Add all the data from the writing map */
        for( itr = m_write_map.begin(); itr != m_write_map.end() ; ++ itr ) {
            log.printfln(TRACE,"Adding for output set fd %d", itr->first);

            struct pollfd pfd;
            pfd.revents = 0;
            pfd.fd = itr->first;
            pfd.events = POLLOUT;
            poll_data.push_back(pfd);
        }

        int rc = poll(&poll_data[0], poll_data.size(), 5000);

        if( rc == -1 ) {
            throw CException("Error in poll", rc);
        }

        if( rc != 0 ) {
            log.printfln(DEBUG, "Events detected");
            vector<struct pollfd>::iterator vitr;

            for( vitr = poll_data.begin(); vitr != poll_data.end(); ++ vitr ) {
                if( vitr->revents != 0 ) {

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
                            unsubscribe( vitr->fd );
                        } else {
                            /* This file descriptor had an event.
                             * Fire that event */
                            fireEvent(vitr->fd, vitr->revents);
                        }
                    }

                }
            }
        }

    }
}

bool FileCollection::unsubscribe(int fd, map_T& a_map) {
    map_T::iterator itr = a_map.find(fd);

    if( itr == a_map.end() ) {
        return false;
    } else {
        FileCollectionObserver* observer = itr->second;
        m_map.erase(itr);
        dealloc_map_T::iterator ditr = m_deallocators.find(observer);

        if( ditr != m_deallocators.end() ) {
            ditr->second->deallocate(observer);
            m_deallocators.erase(ditr);
        }
    }

    return true;
}

FileCollection::~FileCollection() {
    close(m_pipe[0]);
    close(m_pipe[1]);
}

}
