#ifndef RINGQUEUE_HPP_
#define RINGQUEUE_HPP_

/*
 * Author: jrahm
 * created: 2015/04/16
 * RingQueue.hpp: <description>
 */

#include <Prelude>

namespace containers {

/**
 * Circular queue implementation
 */
template<class T>
class RingQueue {
public:
    class iterator {
    public:
        iterator(RingQueue& rq, size_t idx):
            idx(idx), inst(&rq) {}
        iterator(){}
        iterator operator+(size_t s) {
            iterator ret = * this;
            ret.idx += s;
        }

        T* operator->() {
            return &inst->get(idx);
        }

        const T* operator->() const {
            return &inst->get(idx);
        }

        T& operator*() {
            return inst->get(idx);
        }

        const T& operator*() const {
            return inst->get(idx);
        }

        bool operator==(const iterator& oth) const {
            return (oth.idx % (inst->getMaxSize()+1)) ==
                    (idx % (inst->getMaxSize()+1)) &&
                        (oth.inst == this->inst);
        }
        bool operator!=(const iterator& oth) const {
            return !(*this == oth);
        }
        void operator++() {
            idx ++;
        }

    private:
        size_t idx;
        RingQueue* inst;
    };

    class const_iterator {
    public:
        const_iterator(const RingQueue& rq, size_t idx):
            idx(idx), inst(&rq) {}
        const_iterator(){}

        void operator++() {
            idx ++;
        }
        const_iterator operator+(size_t s) {
            iterator ret = * this;
            ret.idx += s;
        }

        const T* operator->() const {
            return &inst->get(idx);
        }

        const T& operator*() const {
            return inst->get(idx);
        }

        bool operator==(const iterator& oth) const {
            return (oth.idx % inst->size()) == (idx % inst->size()) &&
                    (oth.inst == this->inst);
        }
        bool operator!=(const iterator& oth) const {
            return !(*this == oth);
        }
    private:
        size_t idx;
        const RingQueue* inst;
    };

    RingQueue(size_t max_size):
        m_max_size(max_size+1),
        m_current_size(0),
        m_ptr(0),
        m_back(0),
        m_arr(new T[max_size+1]) {
        };

    RingQueue(const RingQueue& oth): m_arr(NULL) {
        *this = oth;
    }

    T& get(size_t idx) {
        if(m_current_size == m_max_size-1) {
            return m_arr[idx % m_max_size];
        }
        return m_arr[idx % m_current_size];
    }

    const T& get(size_t idx) const {
        if(m_current_size == m_max_size-1) {
            return m_arr[idx % m_max_size];
        }
        return m_arr[idx % m_current_size];
    }

    size_t size() {
        return m_current_size;
    }

    size_t getMaxSize() {
        return m_max_size - 1;
    }

    void push_back(const T& next) {
        m_arr[m_back] = next;

        if(m_back == (m_ptr+(m_max_size-1)) % m_max_size ) {
            m_ptr ++;
            m_ptr %= m_max_size;
            m_back ++;
            m_back %= m_max_size;
        } else {
            m_back ++;
            m_back %= m_max_size;
            m_current_size ++;
        }
    }

    T& front() {
        return m_arr[m_ptr];
    }

    const T& front() const {
        return m_arr[m_ptr];
    }

    void operator=(const RingQueue& oth) {
        delete[] m_arr;
        m_arr = new T[oth.m_max_size];
        std::copy(oth.m_arr, oth.m_arr + oth.m_max_size, m_arr);
        m_max_size = oth.m_max_size;
        m_current_size = oth.m_current_size;
        m_ptr = oth.m_ptr;
        m_back = oth.m_back;
    }

    void pop_front() {
        m_ptr ++;
        m_current_size --;
        m_ptr %= m_max_size;
    }

    iterator begin() {
        return iterator(*this, m_ptr);
    }
    const_iterator begin() const {
        return const_iterator(*this, m_ptr);
    }

    iterator end() {
        return iterator(*this, m_back);
    }
    const_iterator end() const {
        return const_iterator(*this, m_back);
    }
    
    ~RingQueue() {
        delete[] m_arr;
    }
private:
    size_t m_max_size;
    size_t m_current_size;

    size_t m_ptr;
    size_t m_back;

    T* m_arr;
};

}

#endif /* RINGQUEUE_HPP_ */
