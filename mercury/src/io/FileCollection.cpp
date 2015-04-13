
#include <sys/poll.h>
#include <unistd.h>

#define HAS_FIONREAD

#include <sys/stat.h>

#ifdef HAS_FIONREAD
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

#include <io/FileCollection.hpp>
#include <io/FilePointer.hpp>
#include <io/NullIO.hpp>

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


/** 
 * The main implementing class of the FileCollection
 */
class FileCollection::Poller: public os::ManagedRunnable {
public:
    Poller(FileCollection& fc): m_super(fc),
        m_log(LogManager::instance().getLogContext("FileCollection", "Poller")),
        m_stop(false) {
            pipe(m_pipe);
            /* Have to be _very_ careful with the Poller log as
             * the log system itself uses the poller and odd stuff
             * may break */
            m_log.redirect(&NullIO::instance(), true);

            /* m_pipe[0] */
            int opts = fcntl(m_pipe[0], F_GETFL);
            opts |= O_NONBLOCK;
            fcntl(m_pipe[0], F_SETFL, opts);

        }

    /**
     *  Fires an event for the file descriptor
     */
    void fireEvent(int fd, int events) {
        FileCollectionObserver* observer = m_map[fd].second;
        HasRawFd* ptr = m_reverse_lookup[fd];
        if(observer) {
            /* observe the change in the file descriptor */
            observer->observe(ptr, events);
        }
    }

    /*
     * Override from Runnable
     */
    void run() {
        /* add for the interrupt */
        struct pollfd pipe_pfd;
        pipe_pfd.revents = 0;
        pipe_pfd.fd = m_pipe[0];
        pipe_pfd.events = subscriptionTypeToBitmask(SUBSCRIBE_READ);

        m_log.printfln(DEBUG, "Start Poller::run()");
        for(;;) {
            handle_commands(); /* Is there something I need to do */
            vector<struct pollfd> poll_data;
            map_T::iterator itr;

            /*
             * Setup the poll structure
             */
            poll_data.push_back(pipe_pfd);
            for(itr = m_map.begin(); itr != m_map.end(); ++ itr) {
                m_log.printfln(TRACE, "Adding %d for subscription", itr->first);
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
                m_log.printfln(TRACE, "BEGIN HandlePollResults");
                if(!handle_poll_results(poll_data)) {
                    break;
                }
                m_log.printfln(TRACE, "END HandlePollResults");
            } else {
                /* We just timed out */
            }
        }

        m_log.printfln(INFO, "Poller has exited");
    }

    void subscribe(HasRawFd* raw_fd,
                   FileCollection::SubscriptionType subscr,
                   FileCollectionObserver* observer,
                   dealloc_T* deallocator) {
        int rfd_int = raw_fd->getRawFd();
        m_log.printfln(DEBUG, "Subscribing %p in file collection", raw_fd);
        rfd_int = raw_fd->getRawFd();
        m_reverse_lookup[rfd_int] = raw_fd;
        m_map[rfd_int] = make_pair(subscr, observer);
        m_deallocators[observer] = deallocator;
   }

   void unsubscribe(HasRawFd* raw_fd) {
        int rfd_int = raw_fd->getRawFd();
        m_reverse_lookup.erase(rfd_int);
        m_log.printfln(DEBUG, "Unsubscribing %p from file collection", raw_fd);

        FileCollectionObserver* obs = m_map[rfd_int].second;

        if(obs) {
            /* Tell the observer the file descriptor
             * was unsubscribed from the collection */
            obs->onUnsubscribe(raw_fd);

            /* Deallocate the observer if we can */
            dealloc_T* dealloc;
            dealloc = m_deallocators[obs];
            if(dealloc) dealloc->deallocate(obs);
            m_deallocators.erase(obs);
        }

        /* Erase observer */
        m_map.erase(rfd_int);
   }

    /*
     * Dispatch a command. That is, interpret the
     * command.
     */
    void dispatch_command(Command* cmd) {
        static const char* arr[] = {"Subscribe", "Unsubscribe", "Exit", "Delete"};
        m_log.printfln(DEBUG, "Dispatching command of type %s", arr[cmd->type]);
        switch(cmd->type) {
            case SUBSCRIBE:
                subscribe(
                    cmd->un.str.raw_fd,
                    cmd->un.str.subscr_typ,
                    cmd->un.str.observer,
                    cmd->un.str.dealloc);
                break;

            case UNSUBSCRIBE:
                unsubscribe(cmd->un.str.raw_fd);
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

    void enqueue_subscribe(HasRawFd* raw_fd,
        FileCollection::SubscriptionType subscr,
        FileCollectionObserver* obs,
        dealloc_T* dealloc) {

        Command* cmd = new Command();
        cmd->type = SUBSCRIBE;
        cmd->un.str.raw_fd = raw_fd;
        cmd->un.str.subscr_typ = subscr;
        cmd->un.str.observer = obs;
        cmd->un.str.dealloc = dealloc;
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

    FileCollection& m_super; /* The owner of this Poller */
    LogContext& m_log;

    int m_pipe[2]; /* Pipeline used to clear */
    bool m_stop; /* true if should stop */
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

void FileCollection::stop() {
    m_poller->stop();
}

void FileCollection::subscribe(SubscriptionType typ, HasRawFd* fd,
                                FileCollectionObserver* observer,
                                dealloc_T* dealloc) {
    m_log.printfln(TRACE, "FileCollection::subscribe");
    m_poller->enqueue_subscribe(fd, typ, observer, dealloc);
    m_poller->interrupt();
}

void FileCollection::unsubscribe(HasRawFd* fd) {
    m_log.printfln(TRACE, "FileCollection::unsubscribe");
    m_poller->enqueue_unsubscribe(fd);
    m_poller->interrupt();
}

}
