#ifndef __SEL4_LINUX_TYPES
#define __SEL4_LINUX_TYPES

#include <stdint.h>

#define bool short

// Map linux integer types from userland
typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef int8_t __s8;
typedef int16_t __s16;
typedef int32_t __s32;
typedef int64_t __s64;

#define u8 __u8
#define u16 __u16
#define u32 __u32
#define u64 __u64

#define s8 __s8
#define s16 __s16
#define s32 __s32
#define s64 __s64

#endif
