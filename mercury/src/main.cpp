#include "MercuryConfig.hpp"

#include <fcntl.h>

byte mercury_magic_cookie[32] = {
    0xe2, 0x1a, 0x14, 0xc8, 0xa2, 0x35, 0x0a, 0x92,
    0xaf, 0x1a, 0xff, 0xd6, 0x35, 0x2b, 0xa4, 0xf3, 
    0x97, 0x79, 0xaf, 0xb5, 0xc1, 0x23, 0x43, 0xf0,
    0xf7, 0x14, 0x17, 0x62, 0x53, 0x4a, 0xa9, 0x7e, 
};

Mutex* g_mutex;
Condition* g_cond;


void sigchld_hdlr(int sig) {
    (void)sig;
    // TODO race condition taken over deadlock. The
    // possible race condition, while very unlikely
    // may cause execution delay for up to 5 mins
    //
    // Consider using a BlockingQueue to signal when
    // children need to be reaped. Or creating a higher
    // layer'd system to handle sigchld events in an
    // observer pattern.
    // ScopedLock __sl(*g_mutex);
    g_cond->signal();
}

void start_logging_service(int fd) {
    /* start a logging service which will read on
     * fd and wait for input */
    pid_t child;
    if(!(child = fork())) {
        LogContext::unlock();

        LogContext& log = LogManager::instance().getLogContext("Child", "StdErr");
        log.printfln(DEBUG, "Error logger started");
        FILE* f = fdopen(fd, "r");
        char read[1024];
        read[1023] = 0;
        while(true) {
            fgets(read, sizeof(read)-1, f);
            log.printfln(ERROR, "%s", read);
        }
    }

    if(child < 0) {
        perror("fork() failed");
    }
}

MonitorProxy* start_monitor(MercuryConfig& conf) {
    return MonitorProxy::startMonitor(conf.monitor_datapoints, conf.monitor_interval);
}

pid_t start_child(byte* recieve, MercuryConfig& config, MonitorProxy* monitor) {
    pid_t ret;

    if(!(ret = fork())) {
        LogContext::unlock();
    
        /* report all stderr to the error pipe */
        fprintf(stderr, "Test stderr\n");
        fflush(stderr);
        /* Child process. Setup the config and boot
         * the merucry state machine */
        mercury::Config conf;
        monitor->readData(conf.monitor_data);
        std::copy(recieve + 32, recieve + 64, conf.mercury_id);
        conf.controller_url = config.controller_url;
        mercury::Proxy& process = mercury::ProxyHolder::instance();
    
        /* start mercury */
        process.start(conf, NULL);
        process.waitForExit();
    
        exit(0);
    }

    return ret;
}

int startMercury(LogContext& m_log, MercuryConfig& config) {
    StreamServerSocket sock;
    SocketAddress* bind_addr = config.bind_ip;

    int error_pipe[2];
    if(pipe(error_pipe)) {
        perror("Error on pipe()\n");
        return 1;
    }

    /* Redirect stderr to the error_pipe. This will make it
     * so we can still see stderr messages in the log */
    dup2(error_pipe[1], STDERR_FILENO);

    /* Start the service that is only resposible for
     * logging the stderr of the this process and
     * its other children */
    start_logging_service(error_pipe[0]);

    /*
     * Start the monitor. This will be used
     * to retrieve statistics about all the
     * network interfaces.
     */
    MonitorProxy* monitor_proxy;
    monitor_proxy = start_monitor(config);

    m_log.printfln(INFO, "Binding to address: %s", bind_addr->toString().c_str());
    sock.bind(*bind_addr);
    sock.listen(1);

    while(true) {
        StreamSocket* client = sock.accept();
        m_log.printfln(INFO, "Connection accepted");

        /* simply get 64 bytes from the client and
         * then close the connection */
        byte recieve[64];
        client->read(recieve, sizeof(recieve));
        delete client;

        /* make sure the cookie is equal to what we expect */
        if(std::equal(recieve, recieve + 32, mercury_magic_cookie)) {
            ScopedLock __sl(*g_mutex);
            /* the cookie is equal to what we expect
             * so continue with the fork() */
            m_log.printfln(INFO, "Magic cookie accepted, forking new process");
            pid_t child;

            child = start_child(recieve, config, monitor_proxy);

            /* parent */
            m_log.printfln(INFO, "Child started pid=%d", child);
            int res = 0;
            pid_t pid;

            /* Wait for the child to exit. this
             * condition will be signaled by the
             * sigchld handler */
            if(!g_cond->timedwait(*g_mutex, 300 SECS)) {
                /* The child is taking too long to return
                 * kill it */
                m_log.printfln(WARN, "Child timeout, sending SIGTERM");
                kill(child, SIGTERM);
                if(!g_cond->timedwait(*g_mutex, 10 SECS)) {
                    /* The child still isn't dead */
                    m_log.printfln(WARN, "Child hard timeout, sending SIGKILL");
                    kill(child, SIGKILL);
                }
            }

            if((pid = waitpid(child, &res, WNOHANG))) {
                if(pid == -1) {
                    m_log.printfln(ERROR, "Error in waitpid %s", strerror(errno));
                } else {
                    m_log.printfln(INFO, "Reap child (%d)\n", (int)pid);
                }
            }

            if(WIFSIGNALED(res)) {
                m_log.printfln(WARN, "Child was signaled with %d", WTERMSIG(res));
            }

            if(WEXITSTATUS(res)) {
                m_log.printfln(WARN, "Child returned abnormal status: %d", WEXITSTATUS(res));
            }

            if(WIFSTOPPED(res)) {
                m_log.printfln(WARN, "Child timed out and was killed by parent");
            }
        } else {
            m_log.printfln(ERROR, "Bad magic cookie received. Timeout for 5 seconds");
            sleep(5);
            m_log.printfln(INFO, "Timeout complete");
        }
    }

    return 0;
}

void sigusr_hdlr(int sig) {
    (void)sig;

    printf("Request to surpress stdout\n");
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDOUT_FILENO);
    dup2(devnull, STDERR_FILENO);
}

int main( int argc, char** argv ) {
    (void) argc ; (void) argv ;

    g_mutex = new Mutex();
    g_cond = new Condition();


    MercuryConfig conf;
    SocketAddress* old = conf.bind_ip;
    signal(SIGCHLD, sigchld_hdlr);
    signal(SIGUSR1, sigusr_hdlr);

    try {
        Json* jsn;
        if(argc > 1) {
            jsn = Json::fromFile(argv[1]);
        } else {
            jsn = Json::fromFile("config.json");
        }
        conf = jsn->convert<MercuryConfig>();
        delete old;
    } catch(Exception& exp) {
        fprintf(stderr, "Unable to load config file: %s\n", exp.getMessage());
    };

    if(conf.log_out) {
        LogManager::instance().addLogOutput(conf.log_out, conf.colors);
    }

    if(conf.logEverything) {
        LogManager::instance().logEverything();
    }

    if(conf.default_level) {
        LogManager::instance().setDefaultLevel(*conf.default_level);
    }

    try {
        if(conf.log_server) {
            LogServer* log_server = new LogServer(*conf.log_server);
            Thread* __th = new Thread(*log_server);
            __th->start();
        }
    
    
        LogContext& m_log = LogManager::instance().getLogContext("Main", "Main");
        m_log.printfln(INFO, "Mercury started");
        return startMercury(m_log, conf);
    } catch(Exception& exp) {
        LogContext& m_log = LogManager::instance().getLogContext("Main", "Main");
        m_log.printfln(FATAL, "Uncaught exception: %s", exp.getMessage());
        return 127;
    }
}
