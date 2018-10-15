#include <linux/module.h>	// included for all kernel modules
#include <linux/init.h>		// included for __init and __exit macros
#include <linux/kernel.h> 	// included for KERN_INFO
#include <linux/proc_fs.h>


int procfile_read(char* buffer, char** buffer_location, off_t offset, int buffer_length, int* eof, void* data) {
	if (offset > 0) {
		return 0;
	}
	else {
		return sprintf(buffer, "Hello, Satan!\n");
	}
}

static int __init mymodule_init(void) {
	struct proc_dir_entry* myentry;

	printk(KERN_INFO "Hello world!\n");

	myentry = create_proc_entry("myproc", 0644, NULL);

	myentry -> read_proc = &procfile_read;

	return 0; 	// Non-zero return means that the module couldn't be loaded.
}

static void __exit mymodule_cleanup(void) {
	remove_proc_entry("myproc", NULL);
}

module_init(mymodule_init);
module_exit(mymodule_cleanup);

