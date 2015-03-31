#ifndef PRIVATE_MERCURY_HPP_
#define PRIVATE_MERCURY_HPP_

/*
 * Author: jrahm
 * created: 2015/03/26
 * Mercury.hpp: <description>
 */

#include "Mercury_StateMachine.hpp"

#include <proc/StateMachine.hpp>
#include <proc/Process.hpp>

#include <io/Inet4Address.hpp>
#include <os/Atomic.hpp>

using namespace proc;
using namespace os;

namespace mercury {
byte mercury_magic_cookie[32] = {
    0xe2, 0x1a, 0x14, 0xc8, 0xa2, 0x35, 0x0a, 0x92,
    0xaf, 0x1a, 0xff, 0xd6, 0x35, 0x2b, 0xa4, 0xf3, 
    0x97, 0x79, 0xaf, 0xb5, 0xc1, 0x23, 0x43, 0xf0,
    0xf7, 0x14, 0x17, 0x62, 0x53, 0x4a, 0xa9, 0x7e, 
};

class Mercury: public MercuryObserver, public Process, public FileCollectionObserver {
public:
    Mercury():
        Process("Mercury"),
        m_mercury_state_machine(this),
        m_state_machine(NULL),
        m_log(LogManager::instance().getLogContext("Process", "Mercury")),
        m_response(NULL){
        }

    void onException(CurlException& expt) {
        m_log.printfln(ERROR, "Error with curl request: %s", expt.getMessage());
        m_state_machine->sendStimulus(BAD_REQUEST);
    }

    void read(const byte* bytes, size_t len) {
        m_response_putter.putBytes(bytes, len);
    }

    void onOK(http_response_code_t status_code) {
        m_log.printfln(INFO, "Curl returned status %d", status_code);
        if(status_code == 200) {
            delete[] m_response;
            m_response = m_response_putter.serialize(m_response_len);
            m_response_putter.clear();
            m_mercury_state_machine.m_response = m_response;
            m_mercury_state_machine.m_response_len = m_response_len;
            m_state_machine->sendStimulus(GOOD_REQUEST);
        } else {
            m_state_machine->sendStimulus(BAD_REQUEST);
        }
    }
    
    void observe(int fd, int events) OVERRIDE {
        (void) events;
        if(fd == m_server_sock.getRawFd()) {
            /* This is a server socket event */
            StreamSocket* client = m_server_sock.accept();
            m_log.printfln(INFO, "Client socket registered %d->%p", client->getRawFd(), client);
            (*m_clients.get())[client->getRawFd()] = client;
            getFileCollection().subscribe(FileCollection::SUBSCRIBE_READ, client, this);
        } else {
            /* this is one of the clients that have done something */
            m_log.printfln(DEBUG, "Witness observation on fd %d", fd);
            StreamSocket* sock = getClientSocket(fd);
            if(!sock) {
                m_log.printfln(WARN, "Event from unknown client socket");
            } else {
                byte l_bytes[64];
                size_t bytes_read = sock->read(l_bytes, 64);
                if(bytes_read != 64) {
                    m_log.printfln(WARN, "Wrong number of bytes read %lu", bytes_read);
                    m_state_machine->sendStimulus(BAD_COOKIE_RECEIVED);
                } else {
                    handle_input_received(l_bytes);
                    /* close and delete client */
                    m_clients.get()->erase(fd);
                    delete sock;

                    getFileCollection().unsubscribe(fd);
                }
            }
        }
    }

    void run() OVERRIDE {
        ScopedLock __sl(m_mutex);
        _run();
    }

    void exit() {
        m_exit_cond.signal();
    }
private:
    void handle_input_received(byte* bytes) {
        byte* id = bytes + 32;
        bool eq = equal(bytes, bytes+32, mercury_magic_cookie);
        if(eq) {
            copy(id, id+32, m_mercury_state_machine.m_id);
            m_state_machine->sendStimulus(COOKIE_RECEIVED);
        } else {
            m_log.printfln(WARN, "Wrong magic cookie found");
            m_state_machine->sendStimulus(BAD_COOKIE_RECEIVED);
        }
    }

    void setup_state_machine() {
        m_state_machine->setEdge(IDLE, COOKIE_RECEIVED, &Mercury_StateMachine::onMagicCookieReceived);
        m_state_machine->setEdge(REQUEST_MADE, GOOD_REQUEST, &Mercury_StateMachine::onGoodRequest);
        m_state_machine->setEdge(REQUEST_MADE, BAD_REQUEST, &Mercury_StateMachine::onBadRequest);
        m_state_machine->setEdge(TIMEOUT, WAIT_TIMEOUT, &Mercury_StateMachine::onWaitTimeoutComplete);
        newThread(*m_state_machine)->start();
    }

    void _run() {
        m_log.printfln(INFO, "Starting mercury");
        m_state_machine = new StateMachine<Stim, Mercury_StateMachine, State>
                                (m_mercury_state_machine, IDLE, getScheduler());
        m_mercury_state_machine.m_state_machine = m_state_machine;
        setup_state_machine();

        Thread* th = newThread(m_mercury_state_machine.m_async_curl);
        th->start();

        bind_server_sock();

        wait_for_exit();
    }

    StreamSocket* getClientSocket(int fd) {
        Atomic<ClientMapT> clients = m_clients.get();
        ClientMapT::iterator itr = clients->find(fd);
        if(itr == clients->end()) {
            return NULL;
        }
        return itr->second;
    }

    void bind_server_sock() {
        Inet4Address addr(INADDR_ANY, 8639);
        m_log.printfln(INFO, "Binding to address %s", addr.toString().c_str());
        m_server_sock.bind(addr);
        m_server_sock.listen();
        getFileCollection().subscribe(FileCollection::SERVER_SOCKET, &m_server_sock, this);
    }

    void wait_for_exit() {
        m_exit_cond.wait(m_mutex);
    }

    Mutex m_mutex;
    Condition m_exit_cond;
    Mercury_StateMachine m_mercury_state_machine;
    StateMachine<Stim, Mercury_StateMachine, State>* m_state_machine; /* Construct this when we run */
    StreamServerSocket m_server_sock;
    LogContext m_log;

    typedef map<int, StreamSocket*> ClientMapT;
    AtomicHolder<ClientMapT> m_clients;

    GlobPutter m_response_putter;
    byte* m_response;
    ssize_t m_response_len;
};

}

#endif /* MERCURY_HPP_ */
