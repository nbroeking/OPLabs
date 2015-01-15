#ifndef ASYNCINPUTSOURCE_HPP_
#define ASYNCINPUTSOURCE_HPP_


#include <io/binary/StreamGetter.hpp>
#include <io/binary/Getter.hpp>
#include <os/Thread.hpp>
#include <vector>

namespace io {

/**
 * @brief A class that allows for abstracted asynchronous reads from a BaseIO.
 *
 * @param T the type of data to read from the BaseIO.
 *          This type must implement the getter methods
 *
 * @see Getter
 *
 * This class implements Runnable, which means it is supposed
 * to be used as a delegate to the Thread class. To use
 * this class, you register callbacks to it which will be called
 * whenever new input has arrived on the given BaseIO
 */
template <class T> /* T must implement the getter methods */
class AsyncInputSource: public os::Runnable {
public:


    /**
     * The interface to be used when reading
     * from the getter.
     */
    class Listener {
    public:
        /**
         * @brief Called when input has arrived
         * @param value the value read from the BaseIO
         */
        virtual void onInputReceived(const T& value) = 0;
    };
    
    /**
     * @brief Creates a new AsyncInputSource from a pre existing getter
     * @param input the getter to delegate
     *
     * This method will take ownership and
     * use pointer-reference notation to set the original
     * pointer to NULL.
     *
     */
    AsyncInputSource(Getter*& input) : m_io(input) {
        input = NULL;
    }
    
    /**
     * @brief Creates a new AsyncInputSource from a pre-existing BaseIO
     * @param bio the BaseIO to delegate
     *
     * This function will take control of the BaseIO passed in, using pointer
     * reference notation to set the original pointer to NULL.
     *
     * This constructor creates a new StreamGetter wrapping bio and using
     * that as the getter
     *
     * @see StreamGetter
     */
    AsyncInputSource(BaseIO*& bio) {
        m_io = new StreamGetter(bio, true);
    }
    
    /**
     * @brief Attach a listener to this input source
     */
    inline void addListener( Listener* listener ) {
        m_listeners.push_back(listener);
    }
    
    /**
     * @brief remove a listener from this input source
     */
    Listener* removeListener( Listener* listener ) {
        m_iterator_t itr = m_listeners.find( listener );
        if( itr != m_listeners.end() ) {
            Listener* ret = *itr;
            m_listeners.erase( itr );
            return ret;
        }
        return NULL;
    }
    
    /**
     * @brief Run this AsyncInputSoure
     */
    void run() OVERRIDE {
        T val;
        int rc;
    
        while ( (rc = getObject(m_io, val)) == 0 ) {
            for( m_iterator_t itr = m_listeners.begin(); itr != m_listeners.end() ; ++ itr ) {
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
