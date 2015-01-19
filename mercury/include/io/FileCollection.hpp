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

namespace io {

class FileCollectionObserver {
public:
    virtual void observe(int fd, int events) = 0;
};

class SingleFileCollectionObserver : public FileCollectionObserver {
public:
    virtual HasRawFd* getFd() = 0;
};
class FileCollection {
public:
    FileCollection();
    
    /**
     * Attach a listener to an object that has a raw file descriptor
     */
    void attach(int fd, FileCollectionObserver* listener);

    void attach(SingleFileCollectionObserver* listener) {
        attach(listener->getFd()->getRawFd(), listener);
    }

    /**
     * Runs a loop of polling
     */
    void run();

    ~FileCollection();
protected:
    void interrupt();

private:

    logger::LogContext* m_log;
    int m_pipe[2];
    void fireEvent( int fd, int events );
    std::map<int, std::vector<FileCollectionObserver*> > m_map;
};

}

#endif /* POLLGROUP_HPP_ */
