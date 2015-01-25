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
#define TEST_INT   0xdeadbeef
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
    char fpath[1024];
    char cwd[1024];
    static const char* testfile = "read_file_test";
    snprintf(fpath, 1024, "%s/%s", getcwd(cwd, 1024), testfile);

	int fd = ::open(fpath, O_WRONLY | O_CREAT, 0600);
	TEST_BOOL("Open", fd > 0);

	FileDescriptor file( fd );
	StreamPutter p( &file );

	p.putInt16be( TEST_SHORT ) ;
	p.putInt32be( TEST_INT   ) ;
	p.putInt64be( TEST_LONG  ) ;

	std::string str(TEST_STRING);
	putObject(p, str);

	return 0;
}

int read_file() {
    char fpath[1024];
    char cwd[1024];
    static const char* testfile = "read_file_test";
    snprintf(fpath, 1024, "%s/%s", getcwd(cwd, 1024), testfile);

	int fd = ::open(fpath, O_RDONLY, 0600);
	TEST_BOOL("Open", fd > 0);

	FileDescriptor file(fd);
	StreamGetter g( &file );

	uint16_t test16;
	uint32_t test32;
	uint64_t test64;

	test16 = g.getInt16be() ;
	test32 = g.getInt32be() ;
	test64 = g.getInt64be() ;

	TEST_EQ_INT( "getInt16be_value", test16, TEST_SHORT );
	TEST_EQ_INT( "getInt32be_value", test32, TEST_INT );
	TEST_EQ( "getInt64be_value", test64, TEST_LONG );

	std::string str;
	getObject(g, str);

	TEST_BOOL( "getString_value", strcmp(str.c_str(), TEST_STRING) == 0 );

    unlink(fpath);
	return 0;
}

int main( int argc, char** argv ) {
    (void) argc;
    (void) argv;
	TEST_FN( test_fd() );
	TEST_FN( write_file() );
	TEST_FN( read_file() );

	return 0;
}
