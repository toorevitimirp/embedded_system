//====================================================================

// File Name : WatchDog_Driver.c

// Function  : X1227 E2PROM Driver Program

// Date      : July 16, 2004

// Version   : 0.0

//====================================================================



#include <linux/init.h>

#include <linux/module.h>

#include <linux/version.h>

#include <linux/types.h>

#include <linux/errno.h>    /* error codes */



#include <asm/uaccess.h>

#include <linux/kernel.h>    /* printk() */

#include <linux/slab.h>

#include <linux/fcntl.h>

#include <linux/fs.h>     /* everything... */

#include <asm/semaphore.h>

#include <asm/arch/cpu_s3c2410.h>

#include <asm/hardware.h>

#include <linux/devfs_fs_kernel.h>


static int __init

WATCHDOG1227_init (void)
{ 	

	Reboot();

	printk ("X1227 WatchDog driver installed OK\n");

	return 0;

}


void __exit

WATCHDOG1227_exit(void)
{

	printk ("X1227 WatchDog driver uninstalled OK\n");



}





module_init(WATCHDOG1227_init);



module_exit(WATCHDOG1227_exit);



MODULE_LICENSE("GPL");

MODULE_AUTHOR("rui zhuo");

MODULE_DESCRIPTION("WATCHDOG Driver for X1227");



