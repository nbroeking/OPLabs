#ifndef MISSING_H_
#define MISSING_H_

/*
 * Author: jrahm
 * created: 2015/04/08
 * missing.h: <description>
 */

#include <string>
#include <sstream>
#include <vector>

inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


inline std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

#endif /* MISSING_H_ */
