/* Emacs linux kernel C mode:-*- linux-c -*-*/

/* Kernel includes */
#include <linux/module.h> 
#include <linux/kernel.h> /* KERN_EMERG */
#include <linux/init.h> /* module_init, modul_exit */

MODULE_LICENSE("GPL");

void base_fun(void)
{
        printk(KERN_EMERG "base1: called base_fun\n");
}


static int
b1_init_module(void)
{
	printk(KERN_INFO "Loaded base1\n");
	return 0;
}

static void
b1_cleanup_module(void)
{
	printk(KERN_INFO "Removed base1\n");
	return;
}

module_init(b1_init_module);
module_exit(b1_cleanup_module);

EXPORT_SYMBOL(base_fun);
