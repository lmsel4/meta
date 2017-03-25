#ifndef __KERNEL_PRINTK__
#define __KERNEL_PRINTK__

#include <stdarg.h>
#include <linux/kern_levels.h>

int printk(const char *fmt, ...);

#endif
