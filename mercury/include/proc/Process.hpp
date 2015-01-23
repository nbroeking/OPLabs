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


/**
 * @brief Address to a process
 */
class ProcessAddress {
public:
    uint32_t ip; /* From the ip address. Not Used */
    uint32_t pid; /* From the process id */
    uint32_t thread; /* From the thread */
};

/**
 * A message can be sent to a process from another
 * process.
 */
class Message {
public:
    inline Message() {}
    inline Message(const Message& oth) :
        from_address( oth.from_address ),
        message( oth.message ) {}

    /* The address of the originating
     * process */
    ProcessAddress from_address;

    /* the array of bytes
     * being sent to the message */
    io::Buffer<byte> message;
};

/**
 * A ProcessProxy is a frontend to the communication with
 * a process.
 */
class ProcessProxy {
public:
    /**
     * @brief Sends a message to the process this proxy represents
     * @param mesg the message to send to the process
     */
    virtual void sendMessage(const ProcessAddress& from,
                              const byte* bytes, size_t len) = 0;
};

/**
 * A class that creates a ProcessProxy from the
 * address of that process.
 */
class ProcessAddressProxy: public ProcessProxy {
public:
    /**
     * @brief Create a new address proxy from the
     * @param addr The address of the process
     */
    ProcessAddressProxy(const ProcessAddress& addr);

    /**
     * @brief Return true if the address in this proxy is valid
     * @return True if this proxy is valid, false otherwise
     */
    virtual bool isValid();
    virtual void sendMessage( const ProcessAddress& from,
                                const byte* bytes, size_t len) OVERRIDE;

private:
    ProcessAddress m_addr;
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
     *
     * @return the name of this process
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

    inline void sendMessage(const ProcessAddress& from, const byte* bytes, size_t len) OVERRIDE {
        Message msg;
        msg.from_address = from;
        msg.message = io::Buffer<byte>(bytes, len);

        inbound_messages.push(msg);
    }

    inline const ProcessAddress& getAddress() {
        return m_id;
    }

    /**
     * @brief Send a message to another process.
     * @param toProcess The process to send to
     * @param bytes The bytes to send
     * @param len The length of the bytes
     */
    void sendTo( ProcessProxy& toProcess, const byte* bytes, size_t len ) ;
protected:
    friend class MessageDigester;
    
    /**
     * @brief Callback called when a message is received from another Process
     * @param from_addr The proxy to the sending process
     * @param bytes The bytes sent
     * @param len The length of the bytes
     */
    virtual void messageReceivedCallback(ProcessAddressProxy& from_addr, const byte* bytes, size_t len) = 0;

private:
    /* Class that digests messages for the
     * process */
    class MessageDigester: public Runnable {
    public:
        Process* super;
        void run() OVERRIDE;
    };

    /* queue of incoming messages to the processes */
    containers::BlockingQueue<Message> inbound_messages;

    io::FileCollection m_file_collection;
    std::string name;
    MessageDigester m_mesg_digest;

    ProcessAddress m_id;
};

}

inline int putObject( io::Putter& putter, const proc::Message& msg ) {
    return putter.putInt32be(msg.from_address.ip)     ||
           putter.putInt32be(msg.from_address.pid)    ||
           putter.putInt32be(msg.from_address.thread) ||
           putObject( putter, msg.message );
}

#endif /* PROCESS_HPP_ */
