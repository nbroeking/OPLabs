#ifndef CONFIG_HPP_
#define CONFIG_HPP_

/*
 * Author: jrahm
 * created: 2015/03/26
 * Config.hpp: <description>
 */

#include <Prelude>

#include <io/SocketAddress.hpp>
    
namespace mercury {

/**
 * @brief Configuration for Mercury
 */
class Config {
public:
    byte mercury_id[32];
    std::string controller_url;
};

}

#endif /* CONFIG_HPP_ */
