#include <os/Condition.hpp>

#include <cerrno>

namespace os {

Condition::Condition() {
    pthread_cond_init( & this->m_condition, NULL ) ;
}

void Condition::signal() {
   pthread_cond_signal( & this->m_condition ) ;
} ;

bool Condition::wait( Mutex& mutex )  {
    int ret = pthread_cond_wait( & this->m_condition, mutex.raw() ) ;
    if(ret) {
        throw ConditionException("Error in wait", ret);
    }
    return true;
} ;

bool Condition::timedwait( Mutex& mutex, timeout_t timeout )  {
    timespec ts ;
    Time::microsInFuture( &ts, timeout ) ;
    int ret = pthread_cond_timedwait( & this->m_condition, mutex.raw(), &ts );
    if(ret == ETIMEDOUT) {
        return false;
    } else if(ret) {
        throw ConditionException("Error in timedwait", ret);
    }
    return true;
} ;

Condition::~Condition() {
    pthread_cond_destroy( & this->m_condition ) ;
}

}
