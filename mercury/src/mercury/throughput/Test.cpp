#include <mercury/throughput/Test.hpp>


#include <fcntl.h>
#include <os/Runnable.hpp>
#include <io/FileCollection.hpp>
#include <proc/Process.hpp>
#include <io/Socket.hpp>
#include <io/DatagramSocket.hpp>
#include <io/DnsPacket.hpp>
#include <io/binary/GlobPutter.hpp>
#include <io/Inet4Address.hpp>

using namespace std;
using namespace io;
using namespace os;
using namespace proc;
using namespace logger;

namespace throughput {

class ThroughputTest: public Process,
                      public FileCollectionObserver,
                      public TestProxy {
public:
    ThroughputTest():
        Process("ThroughputTest"),
        bytes_received(0),
        last_bytes_received(0),
        upload(false),
        m_continue_write(true),
        current_id(0){}


    void observe(HasRawFd* fd, int events) {
        (void)fd; (void)events;
        if(upload) {
            ssize_t bytes_written = m_stream_sock.write(m_buffer, sizeof(m_buffer));
            this->bytes_received += bytes_written;
            m_log->printfln(TRACE, "[%p] Wrote %d bytes: %d\n", this, (int)bytes_written, (int)this->bytes_received);
        } else {
            ssize_t bytes_read = m_stream_sock.read(m_buffer, sizeof(m_buffer));
            this->bytes_received = this->bytes_received + bytes_read;
            m_log->printfln(TRACE, "[%p] Read %d bytes: %d\n", this, (int)bytes_read, (int)this->bytes_received);
        }
    }

    class DnsSender: public Runnable, public FileCollectionObserver {
    public:
        DnsSender(SocketAddress& addr, LogContext& log, FileCollection& fc):
            m_file_collection(fc),
            m_current_id(0),
            m_log(log){
                m_sock.bind(addr);
                fc.subscribe(FileCollection::SUBSCRIBE_READ, &m_sock, this);
            }

        void observe(HasRawFd* sock, int events) {
            (void) sock; (void) events;

            ScopedLock __sl(m_mutex);

            byte resp_id[2];
            m_sock.receive(resp_id, 2, NULL);
            u16_t rid = (resp_id[0] << 8) + resp_id[1];
            timeout_t sent_time = m_sent_times[rid];
            timeout_t recv_time = Time::currentTimeMicros();
            timeout_t latency = recv_time - sent_time;
            m_latency[rid] = latency;

            m_log.printfln(INFO, "Receieve DNS Respose for id=%hu. sent_time=%f, recv_time=%f, latency=%f", rid,
                sent_time / 1000000.0,
                recv_time / 1000000.0,
                latency   / 1000000.0);
        }

        ~DnsSender() {
            m_log.printfln(DEBUG, "Unsubscribing Self");
            m_file_collection.unsubscribe(&m_sock, false);
        }

        void run() {
            static Inet4Address addr("8.8.8.8", 53);

            ScopedLock __sl(m_mutex);
    
            DnsPacket packet("google.com", m_current_id);
            GlobPutter glob;
            putObject(glob, packet);
    
            ssize_t out;
            byte* ser = glob.serialize(out);
    
            m_sock.sendTo(ser, out, addr);
            m_sent_times[m_current_id] = Time::currentTimeMicros();
    
            m_log.printfln(INFO, "[%p] Sent DNS request id=%d", this, m_current_id);
            delete[] ser;
    
            m_current_id = m_current_id + 1;
        }

        void getLatencyMicros(vector<timeout_t>& vec) const {
            map<u16_t,timeout_t>::const_iterator itr;
            vec.clear();
            for(u16_t i = 0; i < m_current_id; ++ i) {
                itr = m_latency.find(i);
                if(itr != m_latency.end()) {
                    vec.push_back(itr->second);
                } else {
                    vec.push_back(-1);
                }
            }
        }

        void clear() {
            m_sent_times.clear();
            m_latency.clear();
            m_current_id = 0;
        }

    private:
        map<u16_t, timeout_t> m_sent_times;
        map<u16_t, timeout_t> m_latency;


        FileCollection& m_file_collection;
        u16_t m_current_id;
        DatagramSocket m_sock;
        LogContext& m_log;

        Mutex m_mutex;
    };

    void periodicPoll() {
        u64_t current = this->bytes_received;
        u64_t delta = current - this->last_bytes_received;

        m_log->printfln(INFO, "[%p] [%f] Poll %d bytes read this second. Total=%lu",
            this, (lu_t)(Time::uptime() / 1000000.0), delta, (u32_t)current);

        download_rate.push_back(delta);
        this->last_bytes_received = current;
    }

    virtual void startTest(const TestConfig& conf, TestObserver* observer) {
        this->m_config = conf;
        this->m_observer = observer;

        m_log->printfln(TRACE, "ThroughputTest - START");
        this->start();
    };

    void reset() {
        bytes_received = 0;
        last_bytes_received = 0;
        download_rate.clear();
        upload = false;
    }

    void run() {
        reset();
        TestResults results;

        m_log->printfln(DEBUG, "Connecting to server address: %s", m_config.server_ip->toString().c_str());
        m_stream_sock.connect(*m_config.server_ip);
        m_stream_sock.setOption(O_NONBLOCK);

        Scheduler& sched = getScheduler();

        MemberFunctionRunner<ThroughputTest> poll_runner
            = MemberFunctionRunner<ThroughputTest>
                (&ThroughputTest::periodicPoll, *this);

        Inet4Address bind_addr("0.0.0.0", 0);
        DnsSender send_runner(bind_addr, *m_log, getFileCollection());

        current_id = 0;

        m_log->printfln(INFO, "this -> [%p]", this);
        u64_t poll_time = 1 SECS ;
        for(; poll_time < 10 SECS; poll_time += 1 SECS) {
            sched.schedule(&poll_runner, poll_time);
        }

        for(poll_time = 200 MILLIS; poll_time < 10 SECS; poll_time += 200 MILLIS) {
            sched.schedule(&send_runner, poll_time);
        }

        getFileCollection().subscribe(FileCollection::SUBSCRIBE_READ,
                                        &m_stream_sock, this);
        Time::sleep(10 SECS);

        /* unsubscribe and do not notify the callback mechanism */
        getFileCollection().unsubscribe(&m_stream_sock, false);
        results.download_throughput_per_sec = download_rate;
        send_runner.getLatencyMicros(results.download_latency_micros);

        /* reset and get ready to do the upload */
        send_runner.clear();
        bytes_received = 0;
        last_bytes_received = 0;
        download_rate.clear();
        upload = true;
        fill(m_buffer, m_buffer + sizeof(m_buffer), 0x41);

        /* start the upload test */
        Time::sleep(1 SECS);
        poll_time = 1 SECS ;
        for(; poll_time < 10 SECS; poll_time += 1 SECS) {
            sched.schedule(&poll_runner, poll_time);
        }

        for(poll_time = 200 MILLIS; poll_time < 10 SECS; poll_time += 200 MILLIS) {
            sched.schedule(&send_runner, poll_time);
        }
        m_log->printfln(INFO, "Start upload");
        getFileCollection().subscribe(
                                FileCollection::SUBSCRIBE_WRITE,
                                &m_stream_sock, this);

        Time::sleep(10 SECS);

        results.upload_throughput_per_sec = download_rate;
        send_runner.getLatencyMicros(results.upload_latency_micros);

        m_log->printfln(INFO, "Waiting for all jobs to finish");
        sched.setStopOnEmpty(true);
        join(); /* join all active threads in this process */

        m_observer->onTestComplete(results);
    }

private:
    StreamSocket m_stream_sock; /* socket used for dns_response */

    u64_t bytes_received;
    u64_t last_bytes_received;
    bool upload;

    bool m_continue_write;
    u64_t current_id;


    std::vector<u64_t> download_rate;

    TestConfig m_config;
    TestObserver* m_observer;

    byte m_buffer[1024 * 1024];
};

TestProxy* g_th_instance;
TestProxy& Test::instance() {
    if(!g_th_instance) {
        g_th_instance = new ThroughputTest();
    }
    return *g_th_instance;
}

}
