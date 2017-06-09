#include <stdlib.h>

#include <linux/slab.h>

void *__kmalloc(size_t size, gfp_t flags)
{
    void *ptr =  malloc(size);

    if (!ptr)
        return NULL;

    if (GFP_ZERO & flags)
    {
        memset(ptr, 0, size);
    }

    return ptr;
}
