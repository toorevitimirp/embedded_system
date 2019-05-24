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

#define ELECTROMOTOR_MAJOR 140

#define ELECTROMOTOR_6 (ELECTROMOTOR_1 + 6)
#define ELECTROMOTOR_7 (ELECTROMOTOR_1 + 7)

#define electromotor_sle (*(volatile unsigned long *)ELECTROMOTOR_GPACON)
#define electromotor_sle_data (*(volatile unsigned long *)ELECTROMOTOR_GPADATA)

devfs_handle_t devfs_electromotor;

unsigned long ELECTROMOTOR_1;
unsigned long ELECTROMOTOR_GPACON;
unsigned long ELECTROMOTOR_GPADATA;

//unsigned long electromotor_write_addr;

int	electromotor_open(struct inode *, struct file *);
int	electromotor_release(struct inode *, struct file *);
int   electromotor_ioctl(struct inode *, struct file *, unsigned int, unsigned long);

ssize_t electromotor_read(struct file *, char * , size_t );
ssize_t electromotor_write(struct file *, char * , size_t );

static struct file_operations electromotor_fops = {
	open:		electromotor_open,
	read:		electromotor_read,
	write:	        electromotor_write,
	//	ioctl:          electromotor_ioctl,
	release:	electromotor_release,
};


int electromotor_open(struct inode *inode, struct file *filp)
{
	/* select NGCS2 */
	electromotor_sle |= 0x2000;
	electromotor_sle_data &= (~0x2000);
	printk("open ok\n");
	return 0;
}

ssize_t electromotor_read(struct file *fp, char * buf,
                 size_t size)
{
    //put_user(key,buf);
    return 1;
}

ssize_t electromotor_write(struct file *fp, char * buf,
                 size_t size)
{
    char key;
    if (get_user(key, buf))
      return -EFAULT;
//printk("electromotor_write:electromotor_write_addr=0x%x; key = %d\n",ELECTROMOTOR_6,key);		 	
    /* send message to control eletromotor */
	 (*(volatile unsigned char *) ELECTROMOTOR_6) = key;
    //put_user(key,buf);
    return 1;
}

int electromotor_release(struct inode *inode, struct file *filp)
{
//comment by yczhao
//	electromotor_sle &= (~0x2000);
//	electromotor_sle_data |= 0x2000;
	printk("release ok\n");
	return 0;
}

int __init electromotor_init(void)
{
	printk("*********************electromotor_init**************\n");
	ELECTROMOTOR_GPACON = ioremap(0x56000000,4);
	ELECTROMOTOR_GPADATA = ioremap(0x56000004,4);
	
	ELECTROMOTOR_1 = ioremap(0x10000000,8);
	//printk("electromotor_init: electromotor_addr_1: %x\n",ELECTROMOTOR_1);

/* register char device */	
	devfs_electromotor = 
	  devfs_register(NULL, "electromotor", DEVFS_FL_DEFAULT,
			 ELECTROMOTOR_MAJOR, 0,
			 S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
			 &electromotor_fops, NULL);
		  
	return 0;
}

static void __exit electromotor_exit(void)
{
	devfs_unregister(devfs_electromotor);
}

module_init(electromotor_init);
module_exit(electromotor_exit);

//__initcall(electromotor_init);
