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



#define parport_reg_control (*(volatile unsigned long *)PARPORT_GPACON)
#define parport_reg_data (*(volatile unsigned long *)PARPORT_GPADATA)
#define parport_data (*(volatile unsigned long *)PARPORT_DATA)
#define parport_control (*(volatile unsigned long *)PARPORT_CONTROL)
#define parport_state (*(volatile unsigned long *)PARPORT_STATE)
devfs_handle_t devfs_handle_parport=NULL;
static int      g_parport_major=0;
unsigned long PARPORT_DATA,PARPORT_CONTROL,PARPORT_STATE;
unsigned long PARPORT_GPACON;
unsigned long PARPORT_GPADATA;
unsigned char parport_kbuf[2];
//unsigned long parport_write_addr;

int	parport_open(struct inode *, struct file *);
int	parport_release(struct inode *, struct file *);
int   parport_ioctl(struct inode *, struct file *, unsigned int cmd, unsigned long arg);

ssize_t parport_read(struct file *, char * , size_t , loff_t *ppos);
ssize_t parport_write(struct file *, char * , size_t , loff_t *ppos);

static struct file_operations parport_fops = {
	open:		parport_open,
	read:		parport_read,
	write:	        parport_write,
	//	ioctl:          parport_ioctl,
	release:	parport_release,
};


int parport_open(struct inode *inode, struct file *filp)
{
	printk("open parport ok\n");
	return 0;
}

ssize_t parport_read(struct file *fp, char * buf,
                 size_t size, loff_t *ppos)
{
	
	parport_reg_data &= (~0x2000);
   	parport_kbuf[0]= parport_state;
	parport_reg_data |= 0x2000;
	if( copy_to_user (buf, parport_kbuf, 1)){
		return -EFAULT;
	}
	printk("Read %02x from parport ok\n",parport_kbuf[0]);
    	return 1;
}

ssize_t parport_write(struct file *fp, char * buf,
                 size_t size, loff_t *ppos)
{
	if(size<1 || size >2)
		return -EFAULT;
	if (copy_from_user (parport_kbuf, buf, size))
                return -EFAULT;
	
	parport_reg_data &= (~0x2000);
	parport_data = 	parport_kbuf[0];
	if(size==2){
		parport_control = parport_kbuf[1];
	}
	parport_reg_data |= 0x2000;
	printk("Write %02x %02x to parport ok\n",parport_kbuf[0],parport_kbuf[1]);
    	return size;
}

int parport_release(struct inode *inode, struct file *filp)
{
	//parport_reg_control &= (~0x2000);
	printk("release parprot ok\n");
	return 0;
}

int __init parport_init(void)
{
	printk("*********************parport_init**************\n");
	PARPORT_GPACON  = ioremap(0x56000000,4);//gpa13 output
	PARPORT_GPADATA = ioremap(0x56000004,4);
	
	PARPORT_DATA    = ioremap(0x10004000,4);
	PARPORT_CONTROL = ioremap(0x10008000,4);
	PARPORT_STATE   = ioremap(0x1000C000,4);
	//printk("parport_init: parport_addr_1: %x\n",PARPORT_1);
	g_parport_major = devfs_register_chrdev(0, "parport", &parport_fops);	
	devfs_handle_parport = 
	  devfs_register(NULL, "parport", DEVFS_FL_DEFAULT,
			 g_parport_major, 0,
			 S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
			 &parport_fops, NULL);
	printk("******************parport_init ok***********\n");
	parport_reg_control &= (~0x2000);
	  
	return 0;
}

static void __exit parport_exit(void)
{
	if(g_parport_major>0)
    	{
        	devfs_unregister_chrdev(g_parport_major, "parport");
        	devfs_unregister(devfs_handle_parport);
    	}
	iounmap(PARPORT_GPACON);
	iounmap(PARPORT_GPADATA);
	iounmap(PARPORT_DATA);
	iounmap(PARPORT_CONTROL);
	iounmap(PARPORT_STATE);

}

module_init(parport_init);
module_exit(parport_exit);

//__initcall(parport_init);
