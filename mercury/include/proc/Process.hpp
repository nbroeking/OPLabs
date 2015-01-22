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
#include <containers/BlockingQueue.hpp>

namespace proc {

class Terminal : public io::BaseIO {
public:

    ssize_t read(byte* buf, size_t len);
    ssize_t write(const byte* buf, size_t len);

    static std::pair<Terminal, Terminal> createTerminal();

    int close();
private:

    Terminal(int rfd, int wfd) {
        write_fd = wfd;
        read_fd = rfd;
    }

    int write_fd;
    int read_fd;
};


class ProcessAddress {
public:
    uint32_t ip; /* From the ip address. Not Used */
    uint32_t pid; /* From the process id */
    uint32_t thread; /* From the thread */
};

class Message {
public:
    inline Message() {}
    inline Message(const Message& oth) :
        from_address( oth.from_address ),
        to_address( oth.to_address ),
        message( oth.message ) {}

    ProcessAddress from_address;
    ProcessAddress to_address;
    io::Buffer<byte> message;
};

class ProcessProxy {
public:
    virtual void sendMessage(const Message& mesg) = 0;
};


/**
 * A process is a type of Thread that has some extra
 * support for asyncronicity.
 *
 * A process is an entity that is global on a per thread
 * basis.
 */
class Process: public os::Runnable, public ProcessProxy {
public:
    Process(const char* name);

    /**
     * return the name of this process
     */
    inline const char* getName() {
        return name.c_str();
    }

    /**
     * Gets the controlling process of the currently executing
     * thread. May return NULL if the thread has no controlling
     * process
     */
    static Process* getProcess();

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
    os::Thread* newThread(Runnable& runner);

    /* the run method. Starts executing a process */
    virtual void run() = 0;


    /**
     * @brief Starts this process in a new thread
     * @return The new thread created by the process
     */
    os::Thread* start();

    inline void sendMessage(const Message& mesg) OVERRIDE {
        inbound_messages.push(mesg);
    }
protected:
    friend class MessageDigester;
    /** Called when a message is received on the
     * in bound queue */
    virtual void messageReceivedCallback( const Message& msg ) = 0;

private:
    class MessageDigester: public Runnable {
    public:
        Process* super;
        void run() OVERRIDE;
    };
    containers::BlockingQueue<Message> inbound_messages;

    io::FileCollection m_file_collection;
    std::string name;
    MessageDigester m_mesg_digest;
};

}

inline int putObject( io::Putter& putter, const proc::Message& msg ) {
    return putter.putInt32be(msg.from_address.ip)     ||
           putter.putInt32be(msg.from_address.pid)    ||
           putter.putInt32be(msg.from_address.thread) ||
           putter.putInt32be(msg.to_address.ip)       ||
           putter.putInt32be(msg.to_address.pid)      ||
           putter.putInt32be(msg.to_address.thread)   ||
           putObject( putter, msg.message );
}

#endif /* PROCESS_HPP_ */
