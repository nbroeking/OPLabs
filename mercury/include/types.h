#ifndef TYPES_H_
#define TYPES_H_

/*
 * Author: jrahm
 * created: 2014/11/07
 * types.h: <description>
 */

#include <stdint.h>

#define INTERFACE class

#if __cplusplus <= 199711L
#define OVERRIDE
#else
#define OVERRIDE override
#endif

typedef uint8_t byte;

#endif /* TYPES_H_ */
