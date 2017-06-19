#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#undef WNOHANG
#undef WUNTRACED
#undef __inline

#include <seL4/utils.h>

#include <linux/printk.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>

// Compatibility layer for character devices
// (mainly support for registering them)

struct device_list {
    struct cdev* cur;
    struct list_head siblings;
};

static struct device_list all_devs = {
    .cur = NULL,
    .siblings = LIST_HEAD_INIT(all_devs.siblings),
};

void cdev_init(struct cdev *dev, const struct file_operations *ops)
{
    dev->ops = ops;
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
 * Registers a cdev with the kernel
 **/
void cdev_put(struct cdev *p)
{
    assert(p);

    if (!all_devs.cur)
    {
        all_devs.cur = p;
    }
    else
    {
        struct device_list* l = kmalloc(sizeof(struct device_list), GFP_KERNEL);
        assert(l);
        l->cur = p;
        list_add(&l->siblings, &all_devs.siblings);
    }
}

/**
 * We don't really care about children do we?
 **/
int cdev_add(struct cdev *parent, dev_t dev, unsigned x)
{
    unimplemented;
    return EINVAL;
}

void cdev_del(struct cdev *dev)
{
    assert(dev);

    struct device_list* cur = NULL;

    list_for_each_entry(cur, &all_devs.siblings, siblings) {
        if (cur->cur == dev) {
            list_del(&cur->siblings);
            break;
        }
    }

    kfree(dev);
}

// TODO: what is this supposed to do?
void cd_forget(struct inode* node)
{
    unimplemented;
}

void device_destroy(struct class *cls, dev_t dev)
{
    unimplemented;
}

struct device* device_create(struct class *cls, struct device *parent, dev_t dev,
                             void *drvdata, const char* fmt, ...)
{
    unimplemented;
}

struct class* __class_create(struct module* owner, const char* name,
                             struct lock_class_key *k)
{
    unimplemented;
}

void class_destroy(struct class *cls)
{
    unimplemented;
}


void class_unregister(struct class *cls)
{
    unimplemented;
}

struct resource * __request_region(struct resource *res,
                                   resource_size_t start,
                                   resource_size_t n,
                                   const char *name, int flags)
{
    unimplemented;
    return NULL;
}

void __release_region(struct resource* res, resource_size_t start, resource_size_t n)
{
    unimplemented;
}

void unregister_chrdev_region()
{
    unimplemented;
}

int register_chrdev_region(int start)
{
    unimplemented;
    return 0;
}
