
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
using namespace containers;


namespace io {

const char* subscribe_str[] = {
    "ServerSocket", "Read", "Write"
};

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

enum CommandType {
    SUBSCRIBE,
    UNSUBSCRIBE,
    EXIT,
    DELETE
};

struct FileCollection::Command {
    CommandType type;

    union {
        struct {
            HasRawFd* raw_fd;
            FileCollection::SubscriptionType subscr_typ;
            FileCollectionObserver* observer;
            Deallocator<FileCollectionObserver>* dealloc;
        } str;
    } un;
};

typedef lang::Deallocator<FileCollectionObserver> dealloc_T;
typedef std::pair<FileCollection::SubscriptionType, FileCollectionObserver*> entry_T;
typedef std::map<int, entry_T> map_T;
typedef std::map<int, HasRawFd*> ptr_map_T;
typedef std::map<FileCollectionObserver*, dealloc_T*> dealloc_map_T;

class FileCollection::Poller: public os::ManagedRunnable {
public:
    Poller(FileCollection& fc): m_super(fc),
        m_log(LogManager::instance().getLogContext("FileCollection", "Poller")) {}

    FileCollection& m_super;
    int m_pipe[2];
    bool m_stop;

    void fireEvent(int fd, int events) {
    }

    void run() {
        m_log.printfln(DEBUG, "Start Poller::run()");
        for(;;) {
            vector<struct pollfd> poll_data;
            map_T::iterator itr;

            for(itr = m_map.begin(); itr != m_map.end(); ++ itr) {
                struct pollfd pfd;
                pfd.revents = 0;
                pfd.fd = itr->first;
                pfd.events = subscriptionTypeToBitmask(itr->second.first);
                poll_data.push_back(pfd);
            }

            /* Poll for 5000 ms */
            int rc = poll(&poll_data[0], poll_data.size(), 5000);
            if(rc == -1) {
                throw CException("Error in poll", rc);
            }

            if(rc != 0) {
                /* there was a file descriptor with an interrupt */
                if(!handle_poll_results(poll_data)) {
                    return;
                }
            }
        }
    }

    void dispatch_command(Command* cmd) {
        HasRawFd* rfd;
        int rfd_int;
        switch(cmd->type) {
            case SUBSCRIBE:
                rfd = cmd->un.str.raw_fd;
                m_log.printfln(DEBUG, "Subscribing %p in file collection", rfd);
                rfd_int = rfd->getRawFd();
                m_reverse_lookup[rfd_int] = rfd;
                m_map[rfd_int] = make_pair(cmd->un.str.subscr_typ, cmd->un.str.observer);
                break;

            case UNSUBSCRIBE:
                rfd = cmd->un.str.raw_fd;
                rfd_int = rfd->getRawFd();
                m_log.printfln(DEBUG, "Unsubscribing %p from file collection", rfd);
                m_reverse_lookup.erase(rfd_int);

                if(m_map[rfd_int].second) {
                    m_map[rfd_int].second->onUnsubscribe(rfd);
                }

                m_map.erase(rfd_int);

                break;

            case EXIT:
                m_log.printfln(DEBUG, "Stop command received");
                m_stop = true;
                break;

            case DELETE:
                delete this;
        }

        /* Reached EOL */
        delete cmd;
    }

    void enqueue_unsubscribe(HasRawFd* raw_fd) {
        Command* cmd = new Command();
        cmd->type = UNSUBSCRIBE;
        cmd->un.str.raw_fd = raw_fd;
        m_command_queue.push(cmd);
    }

    void enqueue_subscribe(HasRawFd* raw_fd, FileCollection::SubscriptionType subscr, FileCollectionObserver* obs) {
        Command* cmd = new Command();
        cmd->type = SUBSCRIBE;
        cmd->un.str.raw_fd = raw_fd;
        cmd->un.str.subscr_typ = subscr;
        cmd->un.str.observer = obs;
        m_command_queue.push(cmd);
    }

    void enqueue_stop() {
        Command* cmd = new Command();
        cmd->type = EXIT;
        m_command_queue.push(cmd);
    }

    void stop() {
        enqueue_stop();
        interrupt();
    }

    void delete_self() {
        Command* cmd = new Command();   
        cmd->type = DELETE;
        m_command_queue.push(cmd);
        interrupt();
    }

    void handle_commands() {
        while(!m_command_queue.empty()) {
            Command* cmd = m_command_queue.front();
            m_command_queue.pop();
            dispatch_command(cmd);
        }
    }

    bool handle_poll_results(std::vector<struct pollfd>& poll_data) {
        vector< pair<int,int> > events;
    
        m_log.printfln(DEBUG, "Events detected");
        vector<struct pollfd>::iterator vitr;
    
        for( vitr = poll_data.begin(); vitr != poll_data.end(); ++ vitr ) {
            if( vitr->revents != 0 ) {
    
                entry_T entry = m_map[vitr->fd];
                if( vitr->fd == m_pipe[0] ) {
                    /* There was an interrupt, throw away the
                     * data */
                    m_log.printfln(DEBUG, "There is an interrupt");
                    byte bytes[1024];
                    read(m_pipe[0], bytes, 1024);
                    handle_commands();

                    if(m_stop) return false;
                } else {
                    /* Fire the event for the correct file descriptor */
                    if( vitr->revents & POLLHUP ) {
    
                        /* There was a hangup, we need to remove the
                         * file descriptor */
                        m_log.printfln(DEBUG, "Unsubscribing hungup file descriptor %d", vitr->fd);
                        enqueue_unsubscribe(m_reverse_lookup[vitr->fd]);
                    } else {
                        /* There _might_ be data to read */
#ifdef  HAS_FIONREAD
                        bool not_is_server = entry.first != SERVER_SOCKET;
                        long n_read = 1;
                        int err = 0;
    
                        if( not_is_server && (err = ioctl( vitr->fd, FIONREAD, &n_read )) ){
                            m_log.printfln(WARN, "ioctl failed: %s", strerror(err));
                        }
    
                        if( n_read > 0 ) {
#endif
                            /* This file descriptor had an event.
                             * Fire that event */
                            events.push_back( make_pair(vitr->fd, vitr->revents) );

#ifdef  HAS_FIONREAD
                        } else {
                            m_log.printfln(DEBUG, "Unsubscrbing %d due to EOF", vitr->fd);
                            enqueue_unsubscribe(m_reverse_lookup[vitr->fd]);
                        }
#endif
                    }
                }
    
            }
        }
    
        vector< pair<int,int> >::iterator itr2;
    
        FOR_EACH(itr2, events) {
            fireEvent(itr2->first, itr2->second);
        }
    
        return true;
    }

    /* Call an interrupt to tell poll to wake up */
    void interrupt() {
        m_log.printfln(TRACE, "Interrupt Called");
        byte bytes[1];
        bytes[0] = 0;
        write(m_pipe[1], bytes, 1);
    }

    BlockingQueue<Command*> m_command_queue; /* Set of commands to handle when there is an interrupt */

    dealloc_map_T m_deallocators; /* map of Observers => Destructors */
    map_T m_map; /* map fd (int) -> entry_T */
    ptr_map_T m_reverse_lookup; /* ptr -> int */
    LogContext m_log;
};

/* BEGIN FILECOLLECTION IMPLEMENTATION */
FileCollection::FileCollection():
    m_log(LogManager::instance().getLogContext("IO", "FileCollection")){
    m_poller = new Poller(*this);
}

FileCollection::~FileCollection() {
    m_poller->stop();
    m_poller->delete_self();
}

void FileCollection::run() {
    m_poller->run();
}

void FileCollection::subscribe(SubscriptionType typ, HasRawFd* fd,
                                FileCollectionObserver* observer


}
