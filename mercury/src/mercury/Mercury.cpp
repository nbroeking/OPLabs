#include <mercury/Mercury.hpp>

#include <io/Inet4Address.hpp>
#include <curl/AsyncCurl.hpp>

using namespace io;
using namespace lang;
using namespace curl;

namespace mercury {

Mercury::SocketHandler::SocketHandler(StreamSocket* sock, Mercury* sup):
    m_sock(sock), m_sup(sup) {
}

Mercury::SocketHandler::~SocketHandler() {
    delete m_sock;
}

void Mercury::SocketHandler::observe(int fd, int events) {
    (void) events;
    static logger::LogContext& log =
        logger::LogManager::instance().getLogContext("Mercury","Socket");

    log.printfln(INFO, "Witness observation in fd %d", fd);
    byte bytes[1024];
    size_t bytes_read;
    bytes_read = m_sock->read( bytes, 1024 );
    log.printHex( INFO, bytes, bytes_read );

    if( !strncmp((char*)bytes, "hello\n", bytes_read) ) {
        /* the magic cookie was received */
        MercuryStim stm = COOKIE_RECEIVED;       
        m_sup->m_state_machine.sendStimulus(stm);
    } else {
        /* Something other than the magic cookie was
         * received */
        MercuryStim stm = BAD_COOKIE_RECEIVED;
        m_sup->m_state_machine.sendStimulus(stm);
    }
}

void Mercury::AcceptHandler::observe(int fd, int events) {
    m_sup->m_log.printfln(INFO, "Read event observed on file descriptor %d with events %02x", fd, events);
    StreamServerSocket* sock = &m_sup->m_sock;
    StreamSocket* client = sock->accept();
    m_sup->m_log.printfln(INFO, "cliend = %p", client);

    if( client ) {
        m_sup->getFileCollection().subscribe(
            FileCollection::SUBSCRIBE_READ,
            client, new SocketHandler(client, m_sup),
                lang::std_deallocator<FileCollectionObserver>());
    }
}

MercuryState Mercury::onBadRequest() {
    m_log.printfln(ERROR, "Bad request");
    return IDLE;
}

MercuryState Mercury::onGoodRequest() {
    m_log.printfln(SUCCESS, "Good request");
    m_log.printHex(INFO, &m_buffer_data[0], m_buffer_data.size());
    return IDLE;
}

MercuryState Mercury::onCookieReceived() {
    m_log.printfln(INFO, "Magic cookie was received");

    MercuryCurlObserver* observer = new MercuryCurlObserver();
    observer->m_sup = this;

    m_buffer_data.clear();
    Curl request;
    request.setURL("https://example.com/");
    request.setFollowLocation(true);

    m_log.printfln(INFO, "Sending curl request");
    m_asnc_curl.sendRequest(request, observer,
        lang::std_deallocator<CurlObserver>() );

    return REQUEST_MADE;
}

MercuryState Mercury::onIncorrectCookie() {
    m_log.printfln(ERROR, "Bad magic cookie found! Pause.");
    return IDLE;
}


Mercury::Mercury():
    Process("Mercury"), m_state_machine(* this, IDLE),
    m_log(logger::LogManager::instance().getLogContext("Mercury", "Internal")) {

    m_log.printfln(INFO, "Consttructing Mercury.");
    Inet4Address addr(INADDR_ANY, 8639);

    m_log.printfln(INFO, "Binding to address %s", addr.toString().c_str());
    m_sock.bind( addr );
    m_sock.listen();
    m_accept_handler.m_sup = this;

    setupStateMachine();

    m_log.printfln(INFO, "Subscribing for read" );
    this->getFileCollection().subscribe( FileCollection::SERVER_SOCKET, &m_sock, &m_accept_handler );
}

void Mercury::run() {
    os::Thread* th = this->newThread( m_asnc_curl );
    th->start();
    this->getFileCollection().run();
}

}
