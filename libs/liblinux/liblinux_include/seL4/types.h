#ifndef __SEL4_LINUX_TYPES
#define __SEL4_LINUX_TYPES

#include <asm/posix_types.h>

// Map linux integer types from userland
typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;
typedef unsigned long __u64;

typedef signed char __s8;
typedef signed short __s16;
typedef signed int __s32;
typedef signed long __s64;

#ifndef __kernel_long_t
#define __kernel_long_t __kernel_long_t
typedef long		__kernel_long_t;
typedef unsigned long	__kernel_ulong_t;
#endif

#ifndef __kernel_ino_t
#define __kernel_ino_t __kernel_ino_t
typedef __kernel_ulong_t __kernel_ino_t;
#endif

#ifndef __kernel_mode_t
#define __kernel_mode_t __kernel_mode_t
typedef unsigned int	__kernel_mode_t;
#endif

#ifndef __kernel_pid_t
#define __kernel_pid_t __kernel_pid_t
typedef int		__kernel_pid_t;
#endif

#ifndef __kernel_ipc_pid_t
#define __kernel_ipc_pid_t __kernel_ipc_pid_t
typedef int		__kernel_ipc_pid_t;
#endif

#ifndef __kernel_uid_t
#define __kernel_uid_t __kernel_uid_t
typedef unsigned int	__kernel_uid_t;
typedef unsigned int	__kernel_gid_t;
#endif

#ifndef __kernel_suseconds_t
#define __kernel_suseconds_t __kernel_suseconds_t
typedef __kernel_long_t		__kernel_suseconds_t;
#endif

#ifndef __kernel_daddr_t
#define __kernel_daddr_t __kernel_daddr_t
typedef int		__kernel_daddr_t;
#endif

#ifndef __kernel_uid32_t
#define __kernel_uid32_t __kernel_uid32_t
typedef unsigned int	__kernel_uid32_t;
typedef unsigned int	__kernel_gid32_t;
#endif

#ifndef __kernel_old_uid_t
#define __kernel_old_uid_t __kernel_old_uid_t
typedef __kernel_uid_t	__kernel_old_uid_t;
typedef __kernel_gid_t	__kernel_old_gid_t;
#endif

#ifndef __kernel_old_dev_t
#define __kernel_old_dev_t __kernel_old_dev_t
typedef unsigned int	__kernel_old_dev_t;
#endif

#ifndef __kernel_time_t
#define __kernel_time_t __kernel_time_t
typedef __kernel_long_t __kernel_time_t;
#endif

#ifndef __kernel_uid16_t
#define __kernel_uid16_t __kernel_uid16_t
typedef unsigned short __kernel_uid16_t;
#endif

#ifndef __kernel_gid16_t
#define __kernel_gid16_t __kernel_gid16_t
typedef unsigned short __kernel_gid16_t;
#endif

#ifndef __kernel_timer_t
#define __kernel_timer_t __kernel_timer_t
typedef int __kernel_timer_t;
#endif

#ifndef __kernel_size_t
#define  __kernel_size_t __kernel_size_t
typedef unsigned int __kernel_size_t;
#endif

#ifndef __kernel_off_t
#define __kernel_off_t __kernel_off_t
typedef __kernel_long_t __kernel_off_t;
#endif

#endif
