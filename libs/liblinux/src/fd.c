#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <linux/list.h>
#include <linux/slab.h>

/**
 * This file provides unix like character device access for seL4
 **/

// TODO: implement this in cdev.c
struct cdev* cdev_by_name(const char* name);

struct opened_file {
    int fd;
    char* name;
    struct cdev* device;
    struct list_head list;
};

static int lfd = 1;

static struct opened_file file_root = {
    .fd = 0,
    .name = NULL,
    .device = NULL,
    .list = LIST_HEAD_INIT(&file_root.list)
};

static struct opened_file *alloc_file(const char* name, struct cdev* d)
{
    struct opened_file* f = kmalloc(sizeof(struct opened_file), GFP_KERNEL);

    assert(name);
    assert(d);

    if (!f)
    {
        printk("out of memory: %s", strerror(errno));
        return NULL;
    }

    f->name = kcalloc(sizeof(char), strlen(name), GFP_KERNEL);

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
    struct cdev* current;

    if (!(current = cdev_by_name(name)))
    {
        errno = ENOENT;
        return -1;
    }

    struct opened_file* f = alloc_file(name, current);

    list_add(&f->list, &file_root.list);

    return 0;
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

    return f->device->ops->write(buf, nbyte);
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

    return f->device->ops->read(buf, nbyte);
}
