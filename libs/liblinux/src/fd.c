#undef NULL
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#include <linux/list.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>

/**
 * This file provides unix like character device access for seL4
 **/

struct opened_file {
    int fd;
    char* name;
    struct cdev* device;
    struct list_head list;
    struct file* file;
};

extern char *strerror(int);

struct seL4_Class {
    struct cdev *cdev;
    char name[256];
    dev_t dev;

    struct list_head others;

};

extern struct seL4_Class *device_by_name(const char *name);

static int lfd = 1;

static struct opened_file file_root = {
    .fd = 0,
    .name = NULL,
    .device = NULL,
    .list = LIST_HEAD_INIT(file_root.list)
};

static struct opened_file *alloc_file(const char* name)
{
    assert(name);

    struct opened_file* f = kmalloc(sizeof(*f), GFP_KERNEL);
    struct cdev *d = cdev_alloc();

    if (!f || !d) // TODO free it
    {
        printk("out of memory: %s", strerror(errno));
        return NULL;
    }

    f->name = kcalloc(sizeof(char), strlen(name), GFP_KERNEL);
    f->file = kmalloc(sizeof(struct file), GFP_KERNEL);

    assert(f->name);
    strncpy(f->name, name, strlen(name));
    f->fd = lfd++;
    f->device = d;

    return f;
}

static void free_file(struct opened_file *f)
{
    assert(f);
    assert(f->name);

    list_del(&f->list);
    kfree(f->file);
    kfree(f->name);
    kfree(f);
}

static struct opened_file *file_by_fd(int fd)
{
    struct opened_file *f;

    list_for_each_entry(f, &file_root.list, list)
    {
        if (f->fd == fd)
        {
            return f;
        }
    }

    return NULL;
}

//see: man 3 open
int open(const char* name)
{
    struct opened_file* f = alloc_file(name);
    struct seL4_Class *cls = device_by_name(name);
    struct inode inode = {
        .i_rdev = cls->dev,
    };

    if (!cls)
    {
        return -1;
    }

    f->device = cls->cdev;

    list_add(&f->list, &file_root.list);

    f->device->ops->open(&inode, f->file);

    return f->fd;
}

//see: man 3 close
int close(int fd)
{
    struct opened_file* cur;

    if (!(cur = file_by_fd(fd)))
    {
        errno = EBADF;
        return -1;
    }

    free_file(cur);
    return 0;
}

//see : man 3 write
ssize_t write(int fd, const void* buf, size_t nbyte)
{
    struct opened_file *f;

    f = file_by_fd(fd);

    if (!f)
    {
        errno = EBADF;
        return 0;
    }

    if (!buf)
    {
        errno = EFAULT;
        return 0;
    }

    // TODO: support offset writing
    return f->device->ops->write(f->file, buf, nbyte, 0);
}

// see: man 3 read
ssize_t read(int fd, void *buf, size_t nbyte)
{
    struct opened_file *f = file_by_fd(fd);

    if (!f)
    {
        errno = EBADF;
        return 0;
    }

    if (!buf)
    {
        errno = EFAULT;
        return 0;
    }

    // TODO: support offset reading
    return f->device->ops->read(f->file, buf, nbyte, 0);
}
