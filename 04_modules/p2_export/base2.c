/* Kernel includes */
#include <linux/module.h> 

MODULE_LICENSE("GPL");

void base_fun(void) 
{
	printk(KERN_EMERG "base2: called base_fun\n");
}

static int
b2_init_module(void)
{
	printk(KERN_INFO "Loaded base2\n");
	return 0;
}

static void
b2_cleanup_module(void)
{
	printk(KERN_INFO "Removed base2\n");
	return;
}

module_init(b2_init_module);
module_exit(b2_cleanup_module);

EXPORT_SYMBOL(base_fun);
