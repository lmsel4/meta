#include <linux/types.h>
// FIXME: if this typedef is not there the off_t is undefined
typedef __kernel_off_t off_t;

#include <linux/scatterlist.h>
#include <linux/slab.h>

struct scatterlist *sg_next(struct scatterlist *sg)
{
#ifdef CONFIG_DEBUG_SG
    BUG_ON(sg->sg_magic != SG_MAGIC);
#endif
    if (sg_is_last(sg))
        return NULL;

    sg++;
    if (unlikely(sg_is_chain(sg)))
        sg = sg_chain_ptr(sg);

    return sg;
}
