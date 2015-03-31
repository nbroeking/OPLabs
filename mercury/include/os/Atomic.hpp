#ifndef ATOMIC_HPP_
#define ATOMIC_HPP_

/*
 * Author: jrahm
 * created: 2015/03/26
 * Atomic.hpp: <description>
 */

#include <os/Mutex.hpp>

namespace os {

/**
 * @brief Class that ensures atomic access to an object while in scope
 * @param T the class to have atomic access to
 */
template <class T>
class Atomic {
public:
    /**
     * @brief construct an atomic pointer from a mutex and object reference
     * @param mut the mutex to use
     * @param internal the internal pointer
     */
    Atomic(Mutex& mut, T& internal):
        m_mutex(&mut), internal(&internal) {
        m_mutex->lock();
        nref = new int(1);
    }

    /**
     * @brief set an atomic equal to another
     * @param oth the other
     */
    Atomic& operator=(Atomic& oth) {
        internal = oth.internal;
        m_mutex = oth.m_mutex;
        unref();
        nref = oth.nref;
        *nref ++;
        return * this;
    }

    T* operator->() {
        return internal;
    }

    T& operator*() {
        return *internal;
    }
        
    ~Atomic() {
        unref();
    }
private:
    void unref() {
        (*nref) --;
        if( *nref == 0 ) {
            delete nref;
            m_mutex->unlock();
        }
    }
    Mutex* m_mutex;
    T* internal;
    int* nref;
};

/**
 * @brief class that holds an object and can give out atomic "leases"
 * The atomic holder will return Atomic instances which may be used to
 * regulate race conditions.
 *
 * @param T the type of class to hold
 *
 * @code
 * AtomicHolder< list<int> > holder;
 *
 * void producer() {
 *  while(cond) {
 *   holder.get()->push_back(item);
 *  }
 * }
 *
 * void consumer() {
 *  int item;
 *  while(cond) {
 *     {
 *      Atomic< list<int> > atom = holder.get();
 *      item = atom->front();
 *      atom->pop_front();
 *     }
 *     printf("%d\n", item);
 *  }
 * }
 * @endcode
 */
template <class T>
class AtomicHolder {
public:
    inline AtomicHolder(): internal(){}
    inline AtomicHolder(const T& wrap): internal(wrap){}
    inline Atomic<T> get() {
        return Atomic<T>(m_mutex, internal);
    }

private:
    Mutex m_mutex;
    T internal;
    
};

}

#endif /* ATOMIC_HPP_ */
