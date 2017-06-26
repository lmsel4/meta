#include <priv_loader.h>
#include <linux/slab.h>

extern struct module *priv_module;

void init_module_struct()
{
	priv_module = kmalloc(sizeof(*priv_module), GFP_USER);

	strcpy(priv_module->name, "asd");
}
