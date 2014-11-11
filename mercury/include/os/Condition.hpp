#ifndef CONDITION_HPP_
#define CONDITION_HPP_

/*
 * Author: jrahm
 * created: 2014/01/29
 * Condition.hpp: <description>
 */

#include <pthread.h>
#include <os/Time.hpp>
#include <os/Mutex.hpp>

namespace os {

class Condition {
public:
    Condition() ;

    virtual void signal() ;
    virtual bool wait( Mutex& mutex ) ;

	virtual bool timedwait( Mutex& mutex, timeout_t timeout );

    virtual ~Condition() ;
private:
    pthread_cond_t m_condition ;
} ;

}

#endif /* CONDITION_HPP_ */
