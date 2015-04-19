#include <mercury/monitor/Monitor.hpp>
#include <proc/Process.hpp>
#include <os/Atomic.hpp>
#include <containers/RingQueue.hpp>

#include <log/LogManager.hpp>

#include <sys/types.h>
#include <dirent.h>

using namespace proc;
using namespace json;
using namespace std;
using namespace os;
using namespace io;
using namespace containers;
using namespace logger;

struct DataPoint {
    const char* intf_name;
    u64_t rx_bytes;
    u64_t tx_bytes;
    timeout_t timestamp;
};

template <>
struct JsonBasicConvert<DataPoint> {
    static json::Json convert_from(const DataPoint& dp) {
        Json ret;
        ret.setAttribute("intf", Json::fromString(dp.intf_name));
        ret.setAttribute("rx_bytes", Json::fromInt(dp.rx_bytes));
        ret.setAttribute("tx_bytes", Json::fromInt(dp.tx_bytes));
        ret.setAttribute("timestamp", Json::fromInt(dp.timestamp/1000l));
        return ret;
    }
};

class Monitor: public Process,
               private FileCollectionObserver {
public:

    void observe(HasRawFd* fd, int ev) {
        (void)fd; (void)ev;
        byte arr[1024];
        m_read.read(arr, 1024);

        StringWriter writer;
        writer.setBaseIO(&m_write);
        Atomic<RingQueue<DataPoint> > atom = m_rq_atomic.get();

        writer.print("[");
        RingQueue<DataPoint>::iterator itr;
        Json jsn;

        itr = atom->begin();
        if(itr != atom->end()) {
            jsn = Json::from(*itr);
            writer.printf("%s", jsn.toString().c_str());
            ++ itr;
        }

        for(; itr != atom->end(); ++ itr) {
            jsn = Json::from(*itr);
            writer.printf(",%s", jsn.toString().c_str());
        }

        writer.print("]\n");
    }

    Monitor(size_t n_data_points, timeout_t interval,
            FileDescriptor& read, FileDescriptor& write):
        Process("Monitor"),
        m_rq_atomic( RingQueue<DataPoint>(n_data_points) ),
        m_interval(interval),
        m_write(write),
        m_read(read),
        m_log(LogManager::instance().getLogContext("Monitor", "Main")){}

    void run() {
        collect_interfaces();

        getFileCollection().subscribe(
            FileCollection::SUBSCRIBE_READ,
            &m_read, this);

        while(true) {
            try {
                Time::sleep(m_interval);
                collect_data();
            } catch(Exception& ex) {
                m_log.printfln(ERROR, "Error caught: %s", ex.getMessage());
            }
        }
    }

private:
    void collect_interfaces() {
        DIR* dp;
        struct dirent* ep;
        dp = opendir("/sys/class/net");

        if(dp != NULL) {
            while( (ep = readdir(dp)) ) {
                if(ep->d_name[0] != '.' && strcmp(ep->d_name, "lo")) {
                    /* ignore the loopback interface */
                    m_log.printfln(INFO, "Found interface %s", ep->d_name);
                    interfaces.push_back(ep->d_name);
                }
            }
            closedir(dp);
        }
    }

    void collect_data() {
        vector<string>::iterator itr;
        char path[1024];

        DataPoint datum;

        FOR_EACH(itr, interfaces) {
            try {
                snprintf(path, sizeof(path),
                    "/sys/class/net/%s/statistics/tx_bytes",
                        itr->c_str());
                datum.tx_bytes = simple_read(path);
                snprintf(path, sizeof(path),
                    "/sys/class/net/%s/statistics/rx_bytes",
                        itr->c_str());
                datum.rx_bytes = simple_read(path);
                datum.intf_name = itr->c_str();
                datum.timestamp = Time::currentTimeMicros();

                // m_log.printfln(TRACE,
                //     "Interface: %s; RX: %llu; TX: %llu",
                //     itr->c_str(), datum.rx_bytes, datum.tx_bytes
                // );

                m_rq_atomic.get()->push_back(datum);
            } catch(Exception& e) {
                m_log.printfln(ERROR, "%s", e.getMessage());
            }
        }
    }

    u64_t simple_read(const char* path) {
        FILE* fp = fopen(path, "r");

        if(!fp) {
            throw Exception("Unable to read");
        }

        char buf[128];
        fgets(buf, 128, fp);
        u64_t ret = strtoull(buf, NULL, 10);
        fclose(fp);
        return ret;
    }

    AtomicHolder< RingQueue<DataPoint> > m_rq_atomic;
    timeout_t m_interval;

    /* what we write the json to */
    FileDescriptor m_write;

    /* what we read from */
    FileDescriptor m_read;

    vector<string> interfaces;

    LogContext& m_log;
};

namespace mercury {

class InternalMonitorProxy: public MonitorProxy {
public:
    virtual void readData(string& str) {
        str.clear();
        char buf[1024];
        buf[0] = 1;
        write(write_fd, buf, 1);

        do {
            fgets(buf, 1024, read_fd);
            str += buf;
        } while(strlen(buf) == 1023);
    }

    FILE* read_fd;
    int write_fd;
    pid_t child;
};

MonitorProxy* MonitorProxy::startMonitor(size_t n_data_points, os::timeout_t interval) {
    int client_write[2];
    int client_read[2];

    pipe(client_write);
    pipe(client_read);

    pid_t child;

    if( !(child = fork()) ) {
        LogContext::unlock();
        FileDescriptor read_fd(client_read[0]);
        FileDescriptor write_fd(client_write[1]);

        Monitor monitor(n_data_points, interval, 
            read_fd, write_fd);
        monitor.start();
        monitor.waitForExit();
    } else {
        /* parent */ 
        int write_fd = client_read[1];
        int read_fd = client_write[0];

        InternalMonitorProxy* ret = new InternalMonitorProxy();
        ret->read_fd = fdopen(read_fd, "r");
        ret->write_fd = write_fd;
        ret->child = child;
        return ret;
    }
}

}
