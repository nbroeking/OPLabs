#ifndef FILENOTFOUNDEXCEPTION_HPP_
#define FILENOTFOUNDEXCEPTION_HPP_

/*
 * Author: jrahm
 * created: 2015/04/10
 * FileNotFoundException.hpp: <description>
 */

#include <Prelude>

namespace io {

class FileNotFoundException: public Exception {
public:
    FileNotFoundException(const char* message):
        Exception(message) {}
};

}

#endif /* FILENOTFOUNDEXCEPTION_HPP_ */
