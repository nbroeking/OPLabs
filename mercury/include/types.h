#ifndef TYPES_H_
#define TYPES_H_

/*
 * Author: jrahm
 * created: 2014/11/07
 * types.h: <description>
 */

#include <stdint.h>

#define INTERFACE class
#define ABSTRACT

#if __cplusplus <= 199711L
#define OVERRIDE
#else
#define OVERRIDE override
#endif

#if defined(TARGET_mips)
#define TARGET_BIG_ENDIAN
#else
#define TARGET_LITTLE_ENDIAN
#endif

#if defined(TARGET_mipsel) || defined(TARGET_mips)
#define TARGET_GROUP_mips
/* Mips does not define ssize_t so we need to
 * define it ourselves */
typedef int32_t ssize_t ;
#endif

typedef uint8_t byte;

#endif /* TYPES_H_ */
