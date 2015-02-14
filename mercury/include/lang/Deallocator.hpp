#ifndef DELETER_HPP_
#define DELETER_HPP_

/*
 * Author: jrahm
 * created: 2015/02/13
 * Deleter.hpp: <description>
 */

namespace lang {

template<class T>
/* abstract */ class Deallocator {
public:
    /* Called when something wants to be deleted */
    virtual void deallocate(T* value) = 0;
};

template<class T>
class StandardDeallocator: public Deallocator<T> {
public:

    static StandardDeallocator& instance() {
        static StandardDeallocator* inst;
    
        if ( ! inst )
            inst = new StandardDeallocator();
    
        return *inst;
    }

    virtual void deallocate(T* value) {
        delete value;
    }

private:
    StandardDeallocator() {}
};

template <class T>
StandardDeallocator<T>* std_deallocator() {
    return &StandardDeallocator<T>::instance();
}

template <class T>
class NullDeallocator: public Deallocator<T> {
public:
    static NullDeallocator& instance() {
        static NullDeallocator* inst ;

        if ( ! inst ) inst = new NullDeallocator();
        return *inst;
    }

    virtual void deallocate(T* value) {
        (void) value;
    }

private:
    NullDeallocator() {}
};


}

#endif /* DELETER_HPP_ */
