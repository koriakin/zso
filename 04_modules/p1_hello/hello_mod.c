#include <linux/module.h>

/*
 * To jest najprostszy mozliwy modul. Po prostu dziala.
 * Swoje moduly prosze opierac o kolejne przyklady.
 */

MODULE_LICENSE("GPL");

int hello_init(void)
{
  printk(KERN_INFO "Hello world\n");
  return 0;
}

void hello_cleanup(void)
{
  printk(KERN_INFO "good bye\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
