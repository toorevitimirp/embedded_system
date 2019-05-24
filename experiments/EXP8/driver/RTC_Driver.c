//====================================================================
// File Name : RTC_Driver.c
// Function  : X1227 RTC Driver
// Date      : July 16, 2004
// Version   : 0.0
//====================================================================

#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>    /* error codes */
#include <asm/uaccess.h>  /*copy_to_user or copy from user*/

#include <linux/kernel.h>    /* printk() */
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/fs.h>     /* everything... */
#include <asm/semaphore.h>
#include <asm/arch/cpu_s3c2410.h>
#include <asm/hardware.h>

#define DEVICE_NAME   "rtc"

#define U8 unsigned char
#define U32 unsigned int
devfs_handle_t X1227_devfs_dir;
static unsigned char *dbuf;

extern void _Wr2410Iic(U32 slvAddr,U32 addr,U8 wdata);
extern void _Rd2410Iic(U32 slvAddr,U32 addr,U8 *rdata);
extern void Init_Iic(void);


static ssize_t
rtc_rd(struct file *filep,char * buffer,size_t length,loff_t * off)
{
	int ret = 0,i;
	U8 rdata;

	dbuf = kmalloc(8*sizeof(unsigned char) , GFP_KERNEL);

	Init_Iic();

	/*Second*/   
	_Rd2410Iic(0xde,0x30,&(rdata));
	dbuf[0] = rdata;

	/*Minute*/
	 _Rd2410Iic(0xde,0x31,&(rdata));
        dbuf[1] = rdata;

	/*Hour*/	
	 _Rd2410Iic(0xde,0x32,&(rdata));
        dbuf[2] = rdata;

	/*Date*/	
	 _Rd2410Iic(0xde,0x33,&(rdata));
        dbuf[3] = rdata;

	/*Month*/
	_Rd2410Iic(0xde,0x34,&(rdata));
        dbuf[4] = rdata;

	/*Year*/
	_Rd2410Iic(0xde,0x35,&(rdata));
        dbuf[5] = rdata;

	copy_to_user(buffer,dbuf,6);
	kfree(dbuf);
	return ret;
}


static ssize_t
rtc_wr(struct file *filep,char * buffer,size_t length,loff_t * off)
{
	U8 data;
	int ic , ret = 0;
	unsigned int reg, mreg, lreg;
	dbuf = kmalloc(length*sizeof(char),GFP_KERNEL);

	copy_from_user(dbuf,buffer,length);
	Init_Iic();

	_Wr2410Iic(0xde,0x3f,0x02);
        _Wr2410Iic(0xde,0x3f,0x06);
	
	for(ic = 0;ic < length; ic+=2)
	{
		//mreg = dbuf[ic];
		//lreg = dbuf[ic+1];
		//reg = mreg<<8|lreg;
		reg = dbuf[ic];
		data = dbuf[ic+1];
        	_Wr2410Iic(0xde,reg,data);
	}
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
RTC1227_init (void)
{ 
	X1227_devfs_dir = devfs_mk_dir(NULL, "X1227", NULL);
	if(!X1227_devfs_dir)  
		return -EBUSY;
	
	devfs_register(X1227_devfs_dir,DEVICE_NAME,DEVFS_FL_AUTO_DEVNUM,0,
			0,S_IFCHR|S_IRUGO|S_IWUGO,&Fops,NULL);

	printk ("X1227 RTC driver installed OK\n");
	printk ("Test is starting!\n");
	//Test_Iic();
	return 0;
}


void __exit
RTC1227_exit(void)
{
	devfs_unregister(X1227_devfs_dir);
	printk("X1227 RTC driver uninstalled OK\n");
}


module_init(RTC1227_init);

module_exit(RTC1227_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rui zhuo");
MODULE_DESCRIPTION("RTC Driver for X1227");
