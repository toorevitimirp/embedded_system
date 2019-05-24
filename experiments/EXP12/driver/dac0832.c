/* driver/char/dac0832.c
 *  this is a dac0832 char device driver.
 * Any problem pls contact support@hhcn.com
 */
#include <module.h>
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


#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <asm/arch/hardware.h>
#include <asm/arch/irqs.h>


#define DAC0832_MAJOR 232
/*define the dac0832 major node is 231*/


#define dac0832_sle (*(volatile unsigned long *)DAC_GPACON)
#define dac0832_sle_data (*(volatile unsigned long *)DAC_GPADATA)

unsigned long DAC_GPACON, DAC_GPADATA; 
unsigned long DAC_DATA;
unsigned long DAC_START;

devfs_handle_t devfs_dac;


int  dac0832_open(struct inode *, struct file *);
int  dac0832_release(struct inode *, struct file *);
ssize_t dac0832_read(struct file *, char * , size_t , loff_t *);
ssize_t dac0832_write(struct file *, const char *, size_t , loff_t *);

static struct file_operations dac0832_fops = {
  //        ioctl:          dac0832_ioctl,
        open:           dac0832_open,
        read:           dac0832_read,
        write:          dac0832_write,
        release:        dac0832_release,
};

/*
 * Open/close code for raw IO.
 */
int dac0832_open(struct inode *inode, struct file *filp)
{
        printk("open ok\n");
        return 0;
}

ssize_t dac0832_read(struct file *filp, char * buf,
                 size_t size, loff_t *offp)
{	
	char key;
	//key = (*(volatile unsigned char *) dac_read_addr);
	if(key == 0){
	//printk("dac0832_read:have no data to read\n");
	return 0;
	}
	put_user(key,buf);
	
	return 1;
}

ssize_t dac0832_write(struct file *filp, const char *buf,
                  size_t size, loff_t *offp)
{
   char key;
   if (get_user(key, buf))
     return -EFAULT;
   printk("dac0832_write:dac_write_addr=0x%x; key = %d\n",DAC_DATA,key);

/* start send message and convert */
   (*(volatile unsigned char *) DAC_DATA) = key;
   udelay(100000);
   (*(volatile unsigned char *) DAC_START) = key;
   
   return 1;
}

//__ioremap
int dac0832_release(struct inode *inode, struct file *filp)
{
        printk("release ok\n");
        return 0;
}

int __init dac0832_init(void)
{
	unsigned int bwscon, bankcon2;
  printk("*********************dac0832_init**************\n");

  DAC_GPACON = ioremap(0x56000000,4);
  DAC_GPADATA = ioremap(0x56000004,4);

  DAC_DATA = ioremap(0x10000008,4);
  DAC_START = ioremap(0x10000030,4);
  
  //printk("haha\n");
  
bwscon=(volatile unsigned int)ioremap(0x48000000,4);
  *(volatile unsigned int*)bwscon |= 1<<10;

  bankcon2=(volatile unsigned int)ioremap(0x4800000c,4);
  *(volatile unsigned int*)bankcon2 |= 0x3FF<<4;

  /* select NGCS2 */	
  dac0832_sle |= 0x2000;
  dac0832_sle_data &= (~0x2000);

  /* register char device */
  devfs_dac =
        devfs_register(NULL,"dac0832",DEVFS_FL_DEFAULT,
		       DAC0832_MAJOR, 0,
		       S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
		       &dac0832_fops,NULL);
  return 0;

}

int __init dac0832_exit(void)
{
  devfs_unregister(devfs_dac);
  return 0;
}

/*
__initcall(dac0832_init);
*/
module_init(dac0832_init);
module_exit(dac0832_exit);

