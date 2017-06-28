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
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/module.h>

#define print(level, fmt, ...) fprintf(stderr, level " %s:%d: " fmt "\n", \
                                       __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define debug(fmt, ...) printf("DEBUG", fmt, ##__VA_ARGS__)

// Compatibility layer for character devices
// (mainly support for registering them)

struct device_list {
    struct cdev* cur;
    struct list_head siblings;
};

struct seL4_Class {
    struct cdev *cdev;
    char name[256];
    dev_t dev;

    struct list_head others;

};

static struct device_list all_devs = {
    .cur = NULL,
    .siblings = LIST_HEAD_INIT(all_devs.siblings),
};

static struct seL4_Class seL4_RegisteredClasses = {
    .name = "/dev/default",
    .others = LIST_HEAD_INIT(seL4_RegisteredClasses.others),
    .dev = 0,
    .cdev = NULL,
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

void debug_device_status()
{
    struct seL4_Class *cur;

    list_for_each_entry(cur, &seL4_RegisteredClasses.others, others)
    {
        printf("Device %s with device number %d:\n", cur->name, cur->dev);
        printf("read function at %p\n", cur->cdev->ops->read);
        printf("write function at %p\n ", cur->cdev->ops->write);
    }
}

struct seL4_Class *device_by_name(const char *name)
{
    struct seL4_Class *cls;

    list_for_each_entry(cls, &seL4_RegisteredClasses.others, others)
    {
        if(strncmp(cls->name, name, 256) == 0)
        {
            debug("Found device %s", name);
            return cls;
        }
    }

    debug("Device %s not found!", name);

    return NULL;
}

int cdev_add(struct cdev *parent, dev_t dev, unsigned x)
{
    struct seL4_Class *cur;

    list_for_each_entry(cur, &seL4_RegisteredClasses.others, others)
    {
        if (cur->dev == dev)
        {
            if (cur->cdev != NULL)
            {
                debug("Registered device %d twice!", dev);
                return -EINVAL;
            }

            cur->cdev = parent;
        }
    }

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
    debug("Destroyed device %s\n", cls->name);
}

struct device* device_create(struct class *cls, struct device *parent, dev_t dev,
                             void *drvdata, const char* fmt, ...)
{
    debug("Creating device %s of class %s\n", fmt, cls->name);

    const char* new_fmt = "/dev/%s";

    struct device *device = malloc(sizeof(struct device));

    device->init_name = calloc(sizeof(char), strlen(fmt));
    strncpy(device->init_name, fmt, strlen(fmt));

    struct seL4_Class *new = malloc(sizeof(struct seL4_Class));

    if (!new)
        return NULL;

    snprintf(new->name, 256, new_fmt, fmt);
    new->dev = dev;
    new->cdev = NULL;

    list_add(&new->others, &seL4_RegisteredClasses.others);

    return device;
}

struct class* __class_create(struct module* owner, const char* name,
                             struct lock_class_key *k)
{
    struct class* cls = malloc(sizeof(*cls));

    if (!cls)
        return NULL;

    cls->name = calloc(sizeof(char), strlen(name));
    strncpy(cls->name, name, strlen(name));

    return cls;
}

void class_destroy(struct class *cls)
{
    kfree(cls);
    debug("Attempted to destroy class %s", cls->name);
}

void class_unregister(struct class *cls)
{
    debug("Unregister class %s", cls->name);
}


void __release_region(struct resource* res, resource_size_t start, resource_size_t n)
{
    debug("Releasing region from %u to %u\n", start, start + n);
}

void unregister_chrdev_region(dev_t d, unsigned x)
{
    unimplemented;
}

int register_chrdev_region(dev_t dev, unsigned start, const char *name)
{
    debug("Registered device region for %s\n", name);

    return 0;
}
