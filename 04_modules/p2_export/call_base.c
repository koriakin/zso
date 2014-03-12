/* Kernel includes */
#include <linux/module.h> 

MODULE_LICENSE("GPL");

void base_fun(void); /* symbol eksportowany z modulu base1 lub base2 */

static int 
cb_init_module(void)
{
	printk(KERN_WARNING "call_base: calling base()\n");
	base_fun();
	return 0;
}

static void
cb_cleanup_module(void)
{
	return;
}

module_init(cb_init_module);
module_exit(cb_cleanup_module);
