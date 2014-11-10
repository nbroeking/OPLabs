#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <io/binary/StreamPutter.hpp>
#include <io/binary/StreamGetter.hpp>

#include <io/FileDescriptor.hpp>
#include <test.h>

using namespace io;
using namespace std;

#define TEST_SHORT 0xdead
#define TEST_INT   32
#define TEST_LONG  0xdeadbeefdeadbeefll
#define TEST_STRING "Hello, World!\n"

int test_fd() {
	int rc;

	int fd = ::open("testfifo", O_WRONLY | O_CREAT, 0600);
	TEST_BOOL( "Open", fd > 0 );

	FileDescriptor file(fd);
	rc = file.write((const byte*)"hello\n", 6);

	TEST_EQ_INT( "FileDescriptor", rc, 6 );
	return 0;
}

int write_file() {
	int fd = ::open("/tmp/mytestfile", O_WRONLY | O_CREAT, 0600);
	TEST_BOOL("Open", fd > 0);

	FileDescriptor file( fd );
	StreamPutter p( &file );

	int rc;

	rc = p.putInt16be( TEST_SHORT ) ||
	     p.putInt32be( TEST_INT   ) ||
	     p.putInt64be( TEST_LONG  )  ;

	TEST_EQ_INT("putIntegers", rc, 0);

	std::string str(TEST_STRING);
	rc = putObject(p, str);
	TEST_EQ_INT("putString", rc, 0);

	return 0;
}

int read_file() {
	int fd = ::open("/tmp/mytestfile", O_RDONLY, 0600);
	TEST_BOOL("Open", fd > 0);

	FileDescriptor file(fd);
	StreamGetter g( &file );

	uint16_t test16;
	uint32_t test32;
	uint64_t test64;

	int rc = g.getInt16be( test16 ) ||
			 g.getInt32be( test32 ) ||
			 g.getInt64be( test64 ) ;

	TEST_EQ( "getIntegers", rc, 0 );
	TEST_EQ( "getInt32be_value", test16, TEST_SHORT );
	TEST_EQ( "getInt32be_value", test32, TEST_INT );
	TEST_EQ( "getInt64be_value", test64, TEST_LONG );

	std::string str;
	rc = getObject(g, str);

	TEST_EQ  ( "getString", rc, 0 );
	TEST_BOOL( "getString_value", strcmp(str.c_str(), TEST_STRING) == 0 );

	return 0;
}

int main( int argc, char** argv ) {
	TEST_FN( test_fd() );
	TEST_FN( write_file() );
	TEST_FN( read_file() );

	return 0;
}
