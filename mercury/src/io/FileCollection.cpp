#include <io/FileCollection.hpp>

#include <sys/poll.h>
#include <unistd.h>

using namespace std;
using namespace logger;
namespace io {


FileCollection::FileCollection() {
    pipe( m_pipe );
    m_map[m_pipe[1]].push_back(NULL);
    m_log = &LogManager::instance().getLogContext("IO", "FileCollection::Static");
}

void FileCollection::interrupt() {
    m_log->printfln(TRACE, "Interrupt called");
    byte bytes[1];
	bytes[0] = 0;
    write(m_pipe[0], bytes, 1);
}

void FileCollection::subscribeForRead( int fd, FileCollectionObserver* observer ) {
    m_log->printfln(DEBUG, "Subscribing new file descriptor %d with observer %p for read", fd, observer);
    m_map[fd].push_back(observer);
    interrupt();
}

void FileCollection::subscribeForWrite( int fd, FileCollectionObserver* observer ) {
    m_log->printfln(DEBUG, "Subscribing new file descriptor %d with observer %p for write", fd, observer);
    m_map[fd].push_back(observer);
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

    vector<FileCollectionObserver*>& vec = m_map[fd];
    vector<FileCollectionObserver*>::iterator itr;

    for( itr = vec.begin() ; itr != vec.end() ; ++ itr ) {
        if( * itr ) {
            (*itr)->observe(fd, events);
        }
    }
}

void FileCollection::run() {
    vector<struct pollfd> poll_data;
    LogContext& log = LogManager::instance().getLogContext("IO", "FileCollection::Run");
    log.printfln(DEBUG, "Start FileCollection::run");

    for( ; ; ) {
        map<int, vector<FileCollectionObserver*> >::iterator itr;
        itr = m_map.begin();

        /* Add the poll data from the reading map */
        for( ; itr != m_map.end() ; ++ itr ) {
            struct pollfd pfd;
            pfd.revents = 0;
            pfd.fd = itr->first;
            pfd.events = POLLIN | POLLPRI;
            poll_data.push_back(pfd);
        }

        /* Add all the data from the writing map */
        for( itr = m_write_map.begin(); itr != m_write_map.end() ; ++ itr ) {
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
                    if( vitr->fd == m_pipe[1] ) {
                        /* There was an interrupt, throw away the
                         * data */
                        byte bytes[1024];
                        read(m_pipe[1], bytes, 1024);
                    } else {
                        /* Fire the event for the correct file descriptor */
                        fireEvent(vitr->fd, vitr->revents);
                    }
                }
            }
        }

    }
}

FileCollection::~FileCollection() {
    close(m_pipe[0]);
    close(m_pipe[1]);
}

}
