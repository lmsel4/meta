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
#include <linux/module.h>

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
    printf("Added cdev number %d to hierarchy\n", p->dev);
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

int cdev_add(struct cdev *parent, dev_t dev, unsigned x)
{
    printf("Adding cdev %s (owned by %s) to hierarchy\n", parent->dev,
           parent->owner->name);
    cdev_put(parent);
    return 0;
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
    printf("Destroyed device %s\n", cls->name);
}

struct device* device_create(struct class *cls, struct device *parent, dev_t dev,
                             void *drvdata, const char* fmt, ...)
{
    printf("Creating device of class %s\n", cls->name);

    struct device *device = malloc(sizeof(struct device));

    device->init_name = calloc(sizeof(char), strlen(fmt));
    strncpy(device->init_name, fmt, strlen(fmt));

    return device;
}

struct class* __class_create(struct module* owner, const char* name,
                             struct lock_class_key *k)
{
    struct class* cls = malloc(sizeof(struct class));

    fprintf(stderr, "Created class %s for module %s\n", name, owner->name);

    cls->name = calloc(sizeof(char), strlen(name));
    strncpy(cls->name, name, strlen(name));

    return cls;
}

void class_destroy(struct class *cls)
{
    fprintf(stderr, "Attempted to destroy class %s", cls->name);
}


void class_unregister(struct class *cls)
{
    fprintf("Unregister class %s", cls->name);
}

struct resource * __request_region(struct resource *res,
                                   resource_size_t start,
                                   resource_size_t n,
                                   const char *name, int flags)
{
    printf("Requesting region from %u to %u\n", start, start + n);

    struct resource* out = malloc(sizeof(struct resource));

    out->name = calloc(sizeof(char), strlen(name));
    strncpy(out->name, name, strlen(name));
    out->start = start;
    out->end = start + n;
    out->flags = flags;
    out->parent = res;

    return out;
}

void __release_region(struct resource* res, resource_size_t start, resource_size_t n)
{
    printf("Releasing region from %u to %u\n", start, start + n);
}

void unregister_chrdev_region()
{
    unimplemented;
}

int register_chrdev_region(dev_t dev, int start, char *name)
{
    printf("Registered device region for %s\n", name);

    return 0;
}
