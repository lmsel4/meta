#include <linux/module.h>
#include <linux/kernel.h>

int hello_init(void)
{
	printk(KERN_INFO "Hello world\n");

	return 0;
}

void hello_exit(void)
{
	printk(KERN_INFO "Goodbye world\n");
}

//module_init(hello_init);
//module_exit(hello_exit);
