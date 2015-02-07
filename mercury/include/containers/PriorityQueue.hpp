#ifndef PRIORITYQUEUE_HPP_
#define PRIORITYQUEUE_HPP_

/*
 * Author: jrahm
 * created: 2015/02/06
 * PriorityQueue.hpp: <description>
 */

#include <iostream>
#include <Prelude>
#include <vector>
#include <algorithm>

namespace containers {

template <class Elem_T, class Comparator_T=std::less<Elem_T> >
class PriorityQueue {
public:
    PriorityQueue() {}

    void push(const Elem_T& value) {
        heap.push_back(value);
        reheap();
    }

    Elem_T& front() {
        return heap[0];
    }

    void pop() {
        heap[0] = heap.back();
        heap.pop_back();
        reheap();
    }

    bool remove(const Elem_T& ele) {
        typename std::vector<Elem_T>::iterator itr;
        itr = std::find(heap.begin(), heap.end(), ele);

        if( itr != heap.end() ) {
            * itr = heap.back();
            heap.pop_back();
            reheap();
            return true;
        }

        return false;
    }

    void print_heap(std::ostream& out) {
        for ( size_t i = 0 ; i < heap.size() ; ++ i ) {
            out << heap[i] << ", ";
        }
        out << std::endl;
    }

private:
    void reheap() {
       loose_reheap(0);
    }

    void loose_reheap( size_t elem ) {
        Comparator_T comp;

        if( elem >= heap.size() ) {
            /* This is a leaf node */
            return;
        }

        /* loose reheap the elements */
        loose_reheap( elem * 2 + 1);
        loose_reheap( elem * 2 + 2 );

        /* true if child is smaller than parent */
        bool comp1 = elem*2+1 >= heap.size() ? false : comp(heap[elem*2+1],heap[elem]);
        bool comp2 = elem*2+2 >= heap.size() ? false : comp(heap[elem*2+2], heap[elem]);

        if( comp1 ) {
            Elem_T tmp = heap[elem];
            heap[elem] = heap[elem*2+1];
            heap[elem*2+1] = tmp;
        } else if ( comp2 ) {
            Elem_T tmp = heap[elem];
            heap[elem] = heap[elem*2+2];
            heap[elem*2+2] = tmp;
        }
    }

    std::vector<Elem_T> heap;
};

}

#endif /* PRIORITYQUEUE_HPP_ */
