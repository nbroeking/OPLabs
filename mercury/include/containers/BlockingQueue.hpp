#ifndef BLOCKINGQUEUE_HPP_
#define BLOCKINGQUEUE_HPP_

/*
 * Author: jrahm
 * created: 2014/01/29
 * BlockingQueue.hpp: <description>
 */

#include <queue>

#include <os/Mutex.hpp>
#include <os/Condition.hpp>
#include <os/ScopedLock.hpp>

namespace containers {

/**
 * @brief A thread-safe blocking queue
 * @param T the data type to store in this queue
 * @param delegate_T the underlying data structure
 *          defaults to a queue, but may be anything
 *          that implements pop and front
 *
 * This is an implementation of a LIFO which is
 * thread safe and blocking. Very useful for
 * communication between threads.
 */
template <class T, class delegate_T=std::queue<T> >
class BlockingQueue {
public:
    /* Construct and initialize a
     * blocking queue */
    BlockingQueue() :
		m_queue(),
        m_mutex(),
        m_condition()
        {}

    /**
     * @brief add a new value to the blocking queue
     * @param val the value to push onto the blocking queue
     *
     * This function will push a new value onto the blocking queue
     * and will also signal any waiting thread, signaling the arrival
     * of a new value.
     */
    void push( const T& val ) {
        os::ScopedLock _sl_( this->m_mutex ) ;
        m_queue.push( val ) ;

        if( m_queue.size() == 1 ) {
            this->m_condition.signal() ;
        }
    }

    /**
     *  @brief return what is on the front of the queue
     *  @return whatever is on the front of the blocking queue
     *
     *  In the case there is nothing on the front of the queue, this
     *  function will block until there is data on the queue. This function
     *  will not pop the element, that must be done by the client.
     */
    T& front( ) {
        os::ScopedLock _sl_( this->m_mutex ) ;

        if( m_queue.empty() ) {
            m_condition.wait( this->m_mutex ) ;
        }

        return m_queue.front() ;
    }

    /** 
     * @brief Timed version of front()
     *
     * @param into where to store the result
     * @param timeout number of microseconds to wait
     *
     * In the case where there is nothing on the blocking queue,
     * this function will block for at most `timeout` milliseconds
     * waiting for a value.
     *
     * The value will be placed in the variable referenced by
     * `into`
     *
     * @return If the timeout was reached, return false otherwise return true
     */
    bool front_timed( T& into, os::timeout_t timeout ) {
        os::ScopedLock _sl_( this->m_mutex ) ;

        if( m_queue.empty() ) {
            if( ! m_condition.timedwait( this->m_mutex, timeout ) ) {
                return false ;
            }
        }

        into = m_queue.front();
        return true ;
    }

    /**
     * @brief like front_timed but with a default value.
     *
     * @param def the default value to return if there is a timeout
     * @param to the timeout in microseconds
     *
     * Return the head of the queue. If the queue is empty, block
     * for at most `to` microseconds. If a new value appears on the
     * queue at that time, return it, otherwise return `def`.
     *
     * @see front_timed(T&,os::timeout_t)
     *
     * @return if there is a timeout, then def otherwise what is on the
     *          front of the queue
     */
    T& front_default( T& def, os::timeout_t to = -1 ) {
        T* tmp = front_timed( to ) ;
        if( ! tmp ) {
            return def ;
        }
        return *tmp ;
    }

    /**
     *  @brief remove the head of this blocking queue
     *
     *  This method is normally called after a flavor of front()
     *  there is no bounds checking, so be careful.
     */
    void pop() {
        os::ScopedLock( this->m_mutex ) ;
        m_queue.pop() ;
    }

    /**
     * @brief test to see if there are no elements in the queue
     */
    inline bool empty() {
        os::ScopedLock( this->m_mutex ) ;
        return m_queue.empty();
    }

    virtual ~BlockingQueue() {}
private:
    delegate_T m_queue ;
    os::Mutex m_mutex ;
    os::Condition m_condition ;
};

}

#endif /* BLOCKINGQUEUE_HPP_ */
