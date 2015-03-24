#include <os/Condition.hpp>

namespace os {

Condition::Condition() {
    pthread_cond_init( & this->m_condition, NULL ) ;
}

void Condition::signal() {
    printf("Condition signal %p\n", &m_condition);
   pthread_cond_signal( & this->m_condition ) ;
} ;

bool Condition::wait( Mutex& mutex )  {
    printf("Condition wait %p\n", &m_condition);
    int ret = !pthread_cond_wait( & this->m_condition, mutex.raw() ) ;
    printf("Wait end\n");

    return ret;
} ;

bool Condition::timedwait( Mutex& mutex, timeout_t timeout )  {
    timespec ts ;
    Time::microsInFuture( &ts, timeout ) ;
    int ret = pthread_cond_timedwait( & this->m_condition, mutex.raw(), &ts );
    return !ret;
} ;

Condition::~Condition() {
    pthread_cond_destroy( & this->m_condition ) ;
}

}
