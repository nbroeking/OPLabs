#include <cstdio>
#include <test.h>
#include <vector>
#include <algorithm>

#include <containers/BlockingQueue.hpp>

using namespace containers;
using namespace os;
using namespace std;

#define TEST_SHORT 0xdead
#define TEST_INT   32
#define TEST_LONG  0xdeadbeefdeadbeefll
#define TEST_STRING "Hello, World!\n"


void blocking_queue_writer(BlockingQueue<int>*& bq) {
	for( int i = 0 ; i < 5 ; ++ i ) {
		bq->push( i );
        Time::sleep( rand() % 1000000 );
	}
}

int test_blocking_queue( BlockingQueue<int>* bq ) {
    for( int i = 0 ; i < 5 ; ++ i ) {
        int test = bq->front();
        bq->pop();

        char name[1024];
        snprintf( name, 1024, "BlockingQueue::front#%d", i );
        TEST_EQ_INT( name, test, i );
    }

    return 0;
}

int test_blocking_queue_stress( BlockingQueue<int>* bq ) {
    vector<int> pulled;

	for ( int i = 0; i < 25; ++ i ) {
        int test = bq->front();
        bq->pop();
        pulled.push_back( test );
    }

    sort( pulled.begin(), pulled.end() );
    int i = 0;
    int rc = 1;
    for( ; i < 5  ; ++ i ) rc &= pulled[i] == 0;
    TEST_EQ("BQStress::Stage1", rc, 1);
    for( ; i < 10 ; ++ i ) rc &= pulled[i] == 1;
    TEST_EQ("BQStress::Stage2", rc, 1);
    for( ; i < 15 ; ++ i ) rc &= pulled[i] == 2;
    TEST_EQ("BQStress::Stage3", rc, 1);
    for( ; i < 20 ; ++ i ) rc &= pulled[i] == 3;
    TEST_EQ("BQStress::Stage4", rc, 1);
    for( ; i < 25 ; ++ i ) rc &= pulled[i] == 4;
    TEST_EQ("BQStress::Stage5", rc, 1);

    return 0;
}

int test_blocking_queue_timeout( BlockingQueue<int>* bq ) {
    int test;
    TEST_EQ( "ShouldTimeout", bq->front_timed( test, 100 ), 1 );

    test = (int)0xdeadbeef;
    bq->push( test );
    test = 0;
    TEST_EQ( "ShouldNotTimeout", bq->front_timed( test, 100 ), 0 );
    TEST_EQ( "ShouldEqualBeef", test, ((int)0xdeadbeef) );

    return 0;
}

int main( int argc, char** argv ) {
    BlockingQueue<int> bq;
    FunctionRunner< BlockingQueue<int>* > 
        runner( blocking_queue_writer, &bq );

    Thread th( runner );
    TEST_EQ( "ThreadStart", th.start(), 0 );
    TEST_FN( test_blocking_queue( &bq ) );
    TEST_EQ( "ThreadJoin", th.join(), 0 );

    Thread* all_threads[5];

    int rc = 0;
    for( int i = 0 ; i < 5; ++ i ) {
        all_threads[i] = new Thread(runner);
        rc |= all_threads[i]->start();
    }

    TEST_EQ( "ThreadsStart", rc, 0 );
    TEST_FN( test_blocking_queue_stress(&bq) );

    for( int i = 0; i < 5; ++ i ) {
        rc |= all_threads[i]->join();
    }

    TEST_EQ( "ThreadsJoin", rc, 0 );

    TEST_FN( test_blocking_queue_timeout(&bq) );

    return 0;
}

