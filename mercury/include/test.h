#ifndef TEST_H_
#define TEST_H_

/*
 * Author: jrahm
 * created: 2014/11/09
 * test.h: <description>
 */

#include <stdio.h>
#ifdef __cplusplus
#include <iostream>
#endif //  __cplusplus

#include <errno.h>
#include <string.h>

#define BOLD "\e[1m"
#define NORM "\e[0m"

#define RED  "\e[1;31m"
#define PURPLE  "\e[1;35m"
#define GREEN  "\e[1;32m"
#define BLUE  "\e[1;34m"

#define LOG( fmt, ... ) \
	    printf(BOLD "%30s" fmt NORM, " +- ", __VA_ARGS__)

#define PRINT_SUCCESS(name) \
		printf(BOLD "%30s" NORM " [" GREEN "PASS" NORM "] " PURPLE "@(%s %s:%d)" NORM "\n", name, __FUNCTION__, __FILE__, __LINE__ ); \

#define PRINT_FAILED(name) \
		printf(BOLD "%30s" NORM " [" RED "FAIL" NORM "] " PURPLE "@(%s %s:%d)" NORM "\n", name, __FUNCTION__, __FILE__, __LINE__ ); \

#define TEST_EQ( name, stmt, eq )\
	if ( (stmt) != (eq) ) { \
		PRINT_FAILED(name) \
		return 1; \
	} else { \
		PRINT_SUCCESS(name) \
	}

#define TEST_BOOL( name, stmt )\
	if ( !(stmt) ) { \
		PRINT_FAILED(name) \
		if(errno) printf(" +- Potential reason: %s\n", strerror(errno));\
		return 1; \
	} else { \
		PRINT_SUCCESS(name) \
	}

#define TEST_EQ_INT( name, stmt, eq )\
	if ( (stmt) != (eq) ) { \
		PRINT_FAILED(name) \
		printf(" +- (expected %d got %d)\n", eq, stmt); \
		if(errno) printf(" +- Potential reason: %s\n", strerror(errno));\
		return 1; \
	} else { \
		PRINT_SUCCESS(name) \
	}

#define TEST_FN( stmt )  \
    printf("\n" BLUE "%30s:\n" NORM, #stmt); \
	if ( (stmt) != 0 ) { \
		PRINT_FAILED(" => ") \
		return 1; \
	} else { \
		PRINT_SUCCESS(" => ") \
	} \
    printf("\n");

#define DECL_TEST( str ) \
	printf( "%s%s%s", BOLD, str ) 

#endif /* TEST_H_ */
