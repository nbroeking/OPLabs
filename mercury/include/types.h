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

typedef uint64_t u64_t;
typedef uint32_t u32_t;
typedef uint16_t u16_t;
typedef uint8_t  u8_t;

typedef int64_t s64_t;
typedef int32_t s32_t;
typedef int16_t s16_t;
typedef int8_t  s8_t;

#endif /* TYPES_H_ */
