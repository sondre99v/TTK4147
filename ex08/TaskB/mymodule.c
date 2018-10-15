#include <linux/module.h>	// included for all kernel modules
#include <linux/init.h>		// included for __init and __exit macros
#include <linux/kernel.h> 	// included for KERN_INFO

static int __init mymodule_init(void){
	printk(KERN_INFO "Hello world!\n");
	return 0; 	// Non-zero return means that the module couldn't be loaded.
}

static void __exit mymodule_cleanup(void){
}

module_init(mymodule_init);
module_exit(mymodule_cleanup);

