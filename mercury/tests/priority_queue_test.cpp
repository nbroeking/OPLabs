#include <containers/PriorityQueue.hpp>
#include <test/Tester.hpp>
#include <algorithm>
#include <valarray>

using namespace std;
using namespace test;
using namespace containers;

bool issorted( std::vector<int>& vec ) {
    int last = vec[0];
    for ( size_t i = 1 ; i < vec.size() ; ++ i ) {
        if( vec[i] < last ) return false;
        last = vec[i];
    }
    return true;
}

int sum( const std::vector<int>& vec ) {
    int sum = 0;
    for ( size_t i = 0 ; i < vec.size() ; ++ i ) {
        sum += vec[i];
    }
    return sum;
}

int main( int argc, char** argv ) {
    Tester::init("PriorityQueue", argc, argv);
    PriorityQueue<int> pq;

    Tester::getLog().printfln(INFO, "Priority queue test");

    std::vector<int> to_rm;

    int i;
    for( size_t j = 0 ; j < 20 ; ++ j ) {
        i = rand() % 10000;

        if( rand() & 1 ) to_rm.push_back(i);

        pq.push(i);
    }
    size_t check = sum( pq.internal() ) + pq.internal().size();
    
    for( size_t j = 0 ; j < to_rm.size() ; ++ j ) {
        Tester::getLog().printfln(INFO, "Remove %d", to_rm[j]);
        pq.remove(to_rm[j]);
        TEST_BOOL("HeapVerify:remove", pq.verify());
    }
    size_t check2 = sum( pq.internal() ) + sum( to_rm ) + pq.internal().size() + to_rm.size();
    
    TEST_BOOL("HeapChecksum", check == check2);

    TEST_BOOL("HeapVerify", pq.verify());

    std::vector<int> vec;

    while ( ! pq.empty() ) {
        i = pq.front();
        pq.pop();
        vec.push_back(i);
        TEST_BOOL("HeapVerify", pq.verify());
    }

    TEST_BOOL("IsSorted", issorted(vec) );
    
    return Tester::instance().exit();
}
