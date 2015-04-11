#ifndef HASRAWFD_HPP_
#define HASRAWFD_HPP_

/*
 * Author: jrahm
 * created: 2015/01/18
 * HasRawFd.hpp: <description>
 */

class HasRawFd {
public:
    virtual int getRawFd() = 0;
    virtual inline ~HasRawFd(){};
};

class HasFdTmpl: public HasRawFd {
public:
    int getRawFd() { return m_fd; };
protected:
    int m_fd;
};

#endif /* HASRAWFD_HPP_ */
