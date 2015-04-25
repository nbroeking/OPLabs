#ifndef MONITOR_HPP_
#define MONITOR_HPP_

/*
 * Author: jrahm
 * created: 2015/04/17
 * Monitor.hpp: <description>
 */

#include <os/Time.hpp>
#include <vector>

#include <json/Json.hpp>
#include <io/FileDescriptor.hpp>

namespace mercury {

class MonitorProxy {
public:
    /**
     * @brief Read json from the monitor process.
     * The json will be a list of objects where the
     * each object is a mapping of interfaces to their
     * total number of bytes read at that moment.
     */
    virtual void readData(std::string& str) = 0;

    /**
     * starts a monitor and returns a proxy to it
     */
    static MonitorProxy* startMonitor(size_t n_data_points, os::timeout_t interval);
};

}

#endif /* MONITOR_HPP_ */
