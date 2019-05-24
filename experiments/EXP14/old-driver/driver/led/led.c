/* driver/char/led.c 
 *  this is a led char device driver. 
 * Any problem pls contact support@hhcn.com
 */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/iobuf.h>
#include <linux/major.h>
#include <linux/blkdev.h>
#include <linux/capability.h>
#include <linux/smp_lock.h>
#include <asm/uaccess.h>
#include <asm/hardware.h>
#include <asm/arch/cpu_s3c2410.h>
#include <asm/io.h>
#include <linux/vmalloc.h>

#include "led_ioctl.h"

#define LED_MAJOR 139


#define LED_2 (LED_1 + 1)
#define LED_3 (LED_1 + 2)
#define LED_4 (LED_1 + 3)
#define LED_5 (LED_1 + 4)
#define LED_6 (LED_1 + 5)


#define led_sle (*(volatile unsigned long *)LED_GPACON)
#define led_sle_data (*(volatile unsigned long *)LED_GPADATA)

devfs_handle_t devfs_led;

unsigned long LED_1;
unsigned long LED_GPACON;
unsigned long LED_GPADATA;

unsigned long led_write_addr;

int	led_open(struct inode *, struct file *);
int	led_release(struct inode *, struct file *);
int   led_ioctl(struct inode *, struct file *, unsigned int, unsigned long);

ssize_t led_read(struct file *, char * , size_t );
ssize_t led_write(struct file *, char * , size_t );


static struct file_operations led_fops = {
	open:		led_open,
	read:		led_read,
	write:	led_write,
   ioctl:   led_ioctl,
	release:	led_release,
};



/* 
 * Open/close code for raw IO.
 */

int led_open(struct inode *inode, struct file *filp)
{

	led_sle |= 0x2000;      //chip_select enable
	led_sle_data &= (~0x2000);//0 --> chip_select
	printk("open ok\n");
	return 0;

}


ssize_t led_read(struct file *fp, char * buf,
                 size_t size)
{
    return 1;
}

ssize_t led_write(struct file *fp, char * buf,
                 size_t size)
{
	char key;
	if (get_user(key, buf))
	    return -EFAULT;
	
	(*(volatile unsigned char *) led_write_addr) = key;
	
        return 1;
}

int led_release(struct inode *inode, struct file *filp)
{
	led_sle &= (~0x2000);   //chip_select disable
	led_sle_data |= 0x2000;
	printk("release ok\n");
	return 0;
}


int led_ioctl(struct inode *inode,
		        struct file *flip,
		        unsigned int command,
		        unsigned long arg)
{
	   int err = 0;

	   switch (command) {    //judge which led want to light
			case IOCTRL_LED_1: 
	   	   		led_write_addr = LED_1;
				break;
			case IOCTRL_LED_2: 
	   	   		led_write_addr = LED_2; 
	      			break;
			case IOCTRL_LED_3: 
	   	   		led_write_addr = LED_3; 
	      			break;
			case IOCTRL_LED_4: 
	   	  		led_write_addr = LED_4; 
	      			break;
			case IOCTRL_LED_5: 
	   	   		led_write_addr = LED_5; 
	      			break;
			case IOCTRL_LED_6: 
	   	  		led_write_addr = LED_6; 
	      			break;
			default:
				err = -EINVAL;
	}
       
        return err;
}


int __init led_init(void)
{

	printk("*********************led_init**************\n");

	LED_GPACON = ioremap(0x56000000,4);
	LED_GPADATA = ioremap(0x56000004,4);
	
	LED_1 = ioremap(0x10000000,8);
	
        devfs_led = devfs_register(NULL, "led", DEVFS_FL_DEFAULT,LED_MAJOR, 0,
	             S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
	             &led_fops, NULL);
		  
	return 0;
}

static void __exit led_exit(void)
{
	devfs_unregister(devfs_led);
}


module_init(led_init);
module_exit(led_exit);

