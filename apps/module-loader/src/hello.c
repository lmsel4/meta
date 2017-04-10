#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

/**
 * Dummy module for testing functionnality ported from Linux
 **/

struct hello_mod {
    int value;
    char* name;
};

static void* ptr;
static hello_mod* array;

int hello_init(void)
{
    static void* zero = NULL;
    printk(KERN_INFO "Hello world\n");

    ptr = kmalloc(sizeof(struct hello_mod), GFP_ATOMIC);

    if (!ptr)
    {
        printk(KERN_CRIT, "Could not allocate memory\n");
        return -1;
    }

    array = kcalloc(sizeof(struct hello_mod), 10, GFP_ATOMIC);

    if (!array)
    {
        printk(KERN_CRIT, "Unable to allocate array memory!\n");
        return -1;
    }

    zero = kzalloc(sizeof(uint64_t), GFP_ATOMIC);

    if (*((uint64_t*) zero) != 0)
    {
        printk(KERN_CRIT, "kzalloc allocated non-zero memory!\n");
        return -1;
    }

    return 0;
}

void hello_exit(void)
{
    kfree(ptr);
    kfree(array);
    printk(KERN_INFO "Goodbye world\n");
}

module_init(hello_init);
module_exit(hello_exit);
