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

/**
 * @brief A class implementing a heap-based priority queue
 * @param Elem_T the elements to store in the priority queue. Must implement operator==
 * @param Comparator_T the comparer between to elements. Must implement operator()(Elem_T, Elem_T)
 */
template <class Elem_T, class Comparator_T=std::less<Elem_T> >
class PriorityQueue {
public:
    PriorityQueue() {}

    /**
     * @brief push an element onto the priority queue
     * @param value The value to push on
     */
    void push(const Elem_T& value) {
        heap.push_back(value);
        sift_up(heap.size() - 1);
    }

    /**
     * @brief return the the element on the front of the queue
     * @return the ldelement on the front of the queue
     */
    Elem_T& front() {
        return heap[0];
    }

    /**
     * pop the front element off the queue
     */
    void pop() {
        heap[0] = heap.back();
        heap.pop_back();
        sift_down(0);
    }

    /**
     * @brief deletes an element from the queue
     * @param ele the element to delete
     * @return true if an element was deleted
     */
    bool remove(const Elem_T& ele) {
        typename std::vector<Elem_T>::iterator itr;
        itr = std::find(heap.begin(), heap.end(), ele);

        if( itr != heap.end() ) {
            size_t index = itr - heap.begin();
            heap[index] = heap.back();
            heap.pop_back();
            reheap(index);
            return true;
        }

        return false;
    }

    /**
     * @brief returns true if the heap is empty, false otherwise
     */
    bool empty() {
        return heap.empty();
    }

    /**
     * @brief should always return true. Verifies the integrity of the queue
     */
    bool verify() {
        return loose_verify(0);
    }

    void print_heap(std::ostream& out) {
        _print_heap(out, 0, 0);
    }

    void _print_heap(std::ostream& out, size_t ele, size_t tab) {
        if( nil(ele) ) return ;

        _print_heap( out, child1(ele), tab + 1 );
        for( size_t i = 0 ; i < tab ; ++ i ) out << "\t";
        out << heap[ele] << std::endl;
        _print_heap( out, child2(ele), tab + 1 );
    }

    const std::vector<Elem_T>& internal() { return heap; }
private:
    bool loose_verify(size_t elem) {
        /* make sure the first child is greater than 
         * the current node*/
        if( elem*2 + 1 < heap.size() ) {
            if( ! comp(heap[elem], heap[elem*2+1]) ) {
                return false;
            }

            /* recursively check the first child */
            if( ! loose_verify(elem*2+1) ) return false;
        }

        /* make sure the socond child is greater
         * than the current node */
        if( elem*2 + 2 < heap.size() ) {
            if( ! comp(heap[elem], heap[elem*2+2]) ) {
                return false;
            }
            /* recursively check the second child */
            if( ! loose_verify(elem*2+2) ) return false;
        }

        return true;
    }

    inline bool   nil   (size_t elem) { return elem > heap.size(); }
    inline size_t child1(size_t elem) { return elem * 2 + 1; }
    inline size_t child2(size_t elem) { return elem * 2 + 2; }
    inline size_t parent(size_t elem) { return (elem-1) / 2; }

    void reheap( size_t elem ) {
        if ( comp(heap[elem], heap[parent(elem)]) ) sift_up( elem ); 
        else sift_down( elem );
    }

    /*
     * Move a heavy element down
     */
    void sift_down(size_t elem) {
        /* go with child 1 */
        size_t ch1_i = child1(elem);
        size_t ch2_i = child2(elem);
        size_t swap_idx;

        if( nil(ch1_i) ) return;
        swap_idx = (nil(ch2_i) || comp(heap[ch1_i], heap[ch2_i])) ? ch1_i : ch2_i;

        if( comp(heap[swap_idx], heap[elem]) ) {
            std::swap( heap[swap_idx], heap[elem] );
            sift_down(swap_idx);
            return;
        }
    }

    /*
     * Move a light element up
     */
    void sift_up( size_t elem ) {
        if ( elem == 0 ) return ;

        size_t parent_i = parent(elem);
        if ( comp(heap[elem], heap[parent_i]) ){
            Elem_T tmp = heap[elem];
            heap[elem] = heap[parent_i];
            heap[parent_i] = tmp;
            sift_up(parent_i);
        }
    }

    Comparator_T comp;
    std::vector<Elem_T> heap;
};

}

#endif /* PRIORITYQUEUE_HPP_ */
