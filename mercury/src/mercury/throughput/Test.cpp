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

using namespace io;
using namespace os;
using namespace proc;

namespace throughput {

class ThroughputTest: public Process,
                      public FileCollectionObserver,
                      public TestProxy {
public:
    ThroughputTest():
        Process("ThroughputTest") {}

    void observe(HasRawFd* fd, int events) {
        if(fd == &m_dns_sock) {
            byte resp_id[2];
            uptr<SocketAddress> from;
            m_dns_sock.receive(resp_id, 2, from.get());

            u16_t rid = (resp_id[0] << 8) + resp_id[1];
            if(rid < this->dns_sent_times.size()) {
                timeout_t sent_time = this->dns_sent_times[rid];
                timeout_t now = Time::currentTimeMicros();
                timeout_t latency = now - sent_time;
                download_latency[rid] = latency;
                m_log->printfln(INFO, "DNS reply for id=%d, latency=%fms", rid, latency/1000.0);
            } else {
                m_log->printfln(WARN, "Unsolicited DNS reply with id=%hu", rid);
            }
        } else if(fd == &m_stream_sock) {
            byte buffer[1024 * 1024];
            u16_t bytes_read = m_stream_sock.read(buffer, sizeof(buffer));
            bytes_received += bytes_read;
        }
    }

    void sendDnsRequest() {
        static Inet4Address addr("8.8.8.8", 53);

        DnsPacket packet("google.com", current_id ++);
        GlobPutter glob;
        putObject(glob, packet);

        ssize_t out;
        byte* ser = glob.serialize(out);

        m_dns_sock.sendTo(ser, out, addr);
        dns_sent_times[current_id] = Time::currentTimeMicros();

        m_log->printfln(INFO, "Sent DNS request id=%d", current_id);
        delete[] ser;
    }

    void periodicPoll() {
        u64_t current = bytes_received;
        u64_t delta = current - last_bytes_received;

        m_log->printfln(INFO, "[%f] Poll %d bytes read this second", Time::uptime() / 1000000.0, delta);

        download_rate.push_back(delta);
        last_bytes_received = current;
    }

    virtual void startTest(const TestConfig& conf, TestObserver* observer) {
        this->m_config = conf;
        this->m_observer = observer;

        this->start();
    };

    void run() {
        m_stream_sock.connect(*m_config.server_ip);
        m_stream_sock.setOption(O_NONBLOCK);

        m_dns_sock.bind(Inet4Address("0.0.0.0", 0));
        m_dns_sock.setNonBlocking(true);

        Scheduler& sched = getScheduler();

        MemberFunctionRunner<ThroughputTest> poll_runner
            = MemberFunctionRunner<ThroughputTest>
                (&ThroughputTest::periodicPoll, *this);
        MemberFunctionRunner<ThroughputTest> send_runner
            = MemberFunctionRunner<ThroughputTest>
                (&ThroughputTest::sendDnsRequest, *this);

        current_id = 0;

        u64_t poll_time = 1 SECS ;
        for(; poll_time < 10 SECS; poll_time += 1 SECS) {
            sched.schedule(&poll_runner, poll_time);
        }

        for(poll_time = 200 MILLIS; poll_time < 10 SECS; poll_time += 200 MILLIS) {
            // sched.schedule(&send_runner, poll_time);
            dns_sent_times.push_back(0);
            download_latency.push_back(-1);
        }

        getFileCollection().subscribe(FileCollection::SUBSCRIBE_READ,
                                        &m_stream_sock, this);
        Time::sleep(10 SECS);
        getFileCollection().unsubscribe(&m_stream_sock);
    }

private:
    DatagramSocket m_dns_sock; /* socket used for latency */
    StreamSocket m_stream_sock; /* socket used for dns_response */

    u64_t bytes_received;
    u64_t last_bytes_received;
    u64_t current_id;


    bool upload;

    std::vector<timeout_t> dns_sent_times;

    std::vector<u64_t> download_rate;
    std::vector<u64_t> download_latency;

    std::vector<u64_t> upload_rate;
    std::vector<u64_t> upload_latency;

    TestConfig m_config;
    TestObserver* m_observer;
};

TestProxy* Test::createInstance() {
    ThroughputTest* ret = new ThroughputTest();
    return ret;
}

}
