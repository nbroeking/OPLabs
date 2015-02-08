#include <containers/PriorityQueue.hpp>
#include <test/Tester.hpp>
#include <algorithm>

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

int main( int argc, char** argv ) {
    Tester::init("PriorityQueue", argc, argv);
    PriorityQueue<int> pq;

    Tester::getLog().printfln(INFO, "Priority queue test");

    int i;
    for( size_t j = 0 ; j < 20 ; ++ j ) {
        i = rand() % 10000;
        pq.push(i);
    }

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
