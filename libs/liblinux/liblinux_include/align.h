#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))
#define __ALIGN_MASK(x, mask) __ALIGN_KERNEL_MASK((x), (mask))

typedef _Bool bool;

#define __kernel_off_t off_t
