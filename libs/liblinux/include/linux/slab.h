#ifndef __SLAB_H

#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

// Needs gfp flags to be defined for module compilation
#include <linux/gfp.h>

/**
 * This mostly a reimplementation of the slab allocator ignoring the flags since
 * most are not implementable
 **/

#define UNUSED __attribute__((unused))
#define LARGE_ALLOC_THRESHOLD (2 << 17)

#define __always_inline __attribute__((always_inline)) inline

/**
 * Required function definition to avoid warning for unused flags
 */
__always_inline void* kmalloc(size_t size, UNUSED int gfp)
{
    return malloc(size);
}

__always_inline void* kcalloc(size_t n, size_t size, UNUSED int gfp)
{
    return calloc(n, size);
}

__always_inline void* kalloc_array(size_t n, size_t size, UNUSED int gfp)
{
    return calloc(n, size);
}

__always_inline void* kzalloc(size_t size, UNUSED int gfp)
{
    void *ptr = NULL;
    ptr = malloc(size);

    if (!ptr)
        return NULL;

    return memset(ptr, 0, size);
}

__always_inline void* krealloc(void *ptr, size_t size, UNUSED int gfp)
{
    return realloc(ptr, size);
}

__always_inline void*  kmalloc_large(size_t size, UNUSED int gfp)
{
    if (size > LARGE_ALLOC_THRESHOLD)
    {
        //TODO: handle large alloc
        errno = ENOSYS;
        return NULL;
    }

    return malloc(size);
}

__always_inline size_t ksize(UNUSED const void *ptr)
{
    errno = ENOSYS;
    return 0;
}

#define kfree(ptr) free(ptr)

#define kmalloc_array(n, size, flags) kcalloc(n, size)
#define kzalloc(size, flags) kcalloc(size, 1)

#endif
