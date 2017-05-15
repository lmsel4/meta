#include <stdio.h>
#include <stdlib.h>

#include <linux/printk.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include <linux/slab.h>

// Compatibility layer for character devices
// (mainly support for registering them)

struct device_list {
    struct cdev* current;
    struct list_head siblings;
    struct list_head children;
};

static struct device_list all_devs = {
    .current = NULL,
    .siblings = LIST_HEAD_INIT(&all_devs.siblings),
    .children = LIST_HEAD_INIT(&all_devs.children)
};

void cdev_init(struct cdev *dev, const struct file_operations *ops)
{
    cdev->ops = ops;
}

/**
 * Allocate a new cdev struct
 **/
struct cdev* cdev_alloc(void)
{
    struct cdev* dev = kmalloc(sizeof(struct cdev), GFP_KERNEL);

    return dev;
}

/**
 * Helper function to find cdev by name for compat'
 **/
const struct cdev* cdev_by_name(const char* name)
{
    struct device_list* current = NULL;

    list_for_each_entry(current, device_list, siblings)
    {
        if (strcmp(current->current->name, name) == 0)
        {
            return current->current;
        }
    }

    return NULL;
}

/**
 * Registers a cdev with the kernel
 **/
void cdev_put(struct cdev *p)
{
    if (!all_devs.current)
    {
        all_devs.current = p;
    }
    else
    {
        struct device_list* l = kmalloc(sizeof(struct device_list), GFP_KERNEL);
        assert(l);
        l->current = p;
        list_add(&l->siblings, &all_devs.siblings);
    }
}

/**
 * We don't really care about children do we?
 **/
int cdev_add(struct cdev *parent, dev_t dev, unsigned x)
{
    printk("%s: unimplemented", __FUNCTION__);
    return EINVAL;
}

void cdev_del(struct cdev *dev)
{
    list_del(&dev->siblings);
    kfree(dev);
}

// TODO: what is this supposed to do?
void cd_forget(struct inode* node)
{
    printk("%s: unimplemented", __FUNCTION__);
}
