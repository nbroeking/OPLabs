#ifndef MISSING_H_
#define MISSING_H_

/*
 * Author: jrahm
 * created: 2015/04/08
 * Missing.h: 
 *  This file contains routines that are basic and yet missing from the d
 *  standard C++ library
 */

#include <string>
#include <sstream>
#include <vector>

#include <cstdarg>

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

inline std::string strprintf(const char* fmt, size_t bufsize, ...) {
    char* buf = new char[bufsize];   
    va_list l;
    va_start(l, bufsize);
    vsnprintf(buf, bufsize, fmt, l);
    std::string ret = buf;
    delete[] buf;
    return ret;
}

#endif /* MISSING_H_ */
