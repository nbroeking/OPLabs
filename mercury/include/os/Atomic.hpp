#ifndef ATOMIC_HPP_
#define ATOMIC_HPP_

/*
 * Author: jrahm
 * created: 2015/03/26
 * Atomic.hpp: <description>
 */

#include <os/Mutex.hpp>

namespace os {

template <class T>
class Atomic {
public:
    Atomic(Mutex& mut, T& internal):
        m_mutex(&mut), internal(&internal) {
        m_mutex->lock();
    }

    void operator=(Atomic& oth) {
        internal = oth.internal;
        m_mutex = oth.m_mutex;
        oth.m_mutex = NULL;
    }

    T* operator->() {
        return internal;
    }

    T& operator*() {
        return *internal;
    }
        
    ~Atomic() {
        if(m_mutex)
            m_mutex->unlock();
    }
private:
    Mutex* m_mutex;
    T* internal;
};

template <class T>
class AtomicHolder {
public:
    inline Atomic<T> get() {
        return Atomic<T>(m_mutex, internal);
    }

private:
    Mutex m_mutex;
    T internal;
    
};

}

#endif /* ATOMIC_HPP_ */
