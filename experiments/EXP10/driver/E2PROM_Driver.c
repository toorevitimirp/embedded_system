//====================================================================
// File Name : E2PROM_Driver.c
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

#define DEVICE_NAME   "e2prom"

#define U8 unsigned char
#define U32 unsigned int

extern void Init_Iic(void);
extern void _Wr2410Iic_E2PROM(U32 slvAddr,U32 addr,U8 *buffer,U32 length);
extern void _Rd2410Iic_E2PROM(U32 slvAddr,U32 addr,U8 *buffer,U32 length);
extern void _Wr2410Iic(U32 slvAddr,U32 addr,U8 wdata);

extern void Test_Iic(void);

devfs_handle_t X1227_devfs_dir;
 
unsigned char *dbuf;
unsigned int major ;


//**********************[Read]*********************************
static ssize_t
rtc_rd(struct file *filep,char * buffer,size_t length,loff_t * off)
{
	int ret = 0;
//	U8 rdata;

	dbuf = kmalloc(length*sizeof(unsigned char) , GFP_KERNEL);

	Init_Iic();
   
	
	_Rd2410Iic_E2PROM(0xaf,0,dbuf,length);

	
	 copy_to_user(buffer,dbuf,length);
	 kfree(dbuf);
	 return ret;
}


static ssize_t
rtc_wr(struct file *filep,char * buffer,size_t length,loff_t * off)
{
//	U8 data;
	int ret = 0;
//	unsigned int reg, mreg, lreg;
	dbuf = kmalloc(length*sizeof(char),GFP_KERNEL);

	copy_from_user(dbuf,buffer,length);
	Init_Iic();

 	_Wr2410Iic(0xde,0x3f,0x02);	
	_Wr2410Iic_E2PROM(0xae,0,dbuf,length);
	
	kfree(dbuf);
	return ret;
		
		
}

static int
rtc_ioctl (struct inode *inode,struct file *file,unsigned int cmd,unsigned long arg)
{
	int rc = 0;
	switch (cmd)
	{
	    default:
		return -ENOIOCTLCMD;
	}//endswitch

	return rc;
}

/*
static int 
rtc_open(struct inode *inode,struct file *file)
{
	
	return 0;
}
*/

static int 
rtc_release (struct inode *inode,struct file *file)
{

	return 0;	
}

struct  file_operations Fops = {
	    owner:THIS_MODULE,
	    read:rtc_rd,
	    write:rtc_wr,
	    ioctl:rtc_ioctl,
//	    open:rtc_open,
	    release:rtc_release,
};


static int __init
E2PROM1227_init (void)
{ 
	X1227_devfs_dir = devfs_mk_dir(NULL, "X1227", NULL);
	if(!X1227_devfs_dir)  
		return -EBUSY;
	
	devfs_register(X1227_devfs_dir,DEVICE_NAME,DEVFS_FL_AUTO_DEVNUM,0,
			0,S_IFCHR|S_IRUGO|S_IWUGO,&Fops,NULL);

	printk ("X1227 E2PROM driver installed OK\n");
	printk ("TestIic is starting!\n");
  	Test_Iic();
	return 0;
}


void __exit
E2PROM1227_exit(void)
{
	devfs_unregister(X1227_devfs_dir);
	printk("X1227 E2PROM driver uninstalled OK\n");
}


module_init(E2PROM1227_init);

module_exit(E2PROM1227_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rui zhuo");
MODULE_DESCRIPTION("E2PROM Driver for X1227");

