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

struct kmem_cache *kmalloc_caches[KMALLOC_SHIFT_HIGH + 1];
struct mem_section mem_section[NR_SECTION_ROOTS][SECTIONS_PER_ROOT];

void *kmem_cache_alloc_trace(struct kmem_cache* c, gfp_t flags, size_t s)
{
    return NULL;
}

void kfree(const void *ptr)
{
    if (ptr)
        free(ptr);
}
