#include <stdlib.h>

#include <linux/slab.h>
#include <linux/gfp.h>

void *__kmalloc(size_t size, gfp_t flags)
{
    void *ptr =  malloc(size);

    if (!ptr)
        return NULL;

    if (__GFP_ZERO & flags)
    {
        memset(ptr, 0, size);
    }

    return ptr;
}
