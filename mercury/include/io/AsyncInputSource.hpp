#ifndef ASYNCINPUTSOURCE_HPP_
#define ASYNCINPUTSOURCE_HPP_

/*
 * Author: jrahm
 * created: 2014/11/21
 * AsyncInputSource.hpp: <description>
 */

#include <io/binary/StreamGetter.hpp>
#include <io/binary/Getter.hpp>
#include <os/Thread.hpp>
#include <vector>

namespace io {

/*
 * A class that allows for input to be passed
 * asynchronously via a callback interface
 */
template <class T> /* T must implement the getter methods */
class AsyncInputSource: public os::Runnable {
public:

/* Interface for listening to the input source */
INTERFACE Listener {
public:
    /* Called once input is received on the pipe */
    virtual void onInputReceived(const T& value) = 0;
};

/* Create a new async source that takes ownership
 * of the getter supplied to it */
AsyncInputSource(Getter*& input) : m_io(input) {
    input = NULL;
}

/* Take control of the async input source that has the
 * stream getter applied to it */
AsyncInputSource(BaseIO*& bio) {
    m_io = new StreamGetter(bio, true);
}

/*
 * Adds a listener to this AsyncInputSource
 */
inline void addListener( Listener* listener ) {
    m_listeners.push_back(listener);
}

Listener* removeListener( Listener* listener ) {
    m_iterator_t itr = m_listeners.find( listener );
    if( itr != m_listeners.end() ) {
        Listener* ret = *itr;
        m_listeners.erase( itr );
        return ret;
    }
    return NULL;
}

void run() OVERRIDE {
    T val;
    int rc;

    while ( (rc = getObject(m_io, val)) == 0 ) {
        for( m_iterator_t itr = m_listeners.begin();
              itr != m_listeners.end() ; ++ itr ) {
            (*itr)->onInputReceived( val );
        }
    }
}

private:

typedef typename std::vector<Listener*>::iterator m_iterator_t;

/* Who is listening? */
std::vector<Listener*> m_listeners;

/* The IO we are wrapping */
Getter* m_io;
};

}

#endif /* ASYNCINPUTSOURCE_HPP_ */
