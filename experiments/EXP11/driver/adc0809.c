/* driver/char/adc0809.c
 *  this is a adc0809 char device driver.
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

#include "adc0809_ioctl.h"

#define ADC0809_MAJOR 231
/*define the adc0809 major node is 231*/


#define adc0809_sle (*(volatile unsigned long *)ADC_GPACON)
#define adc0809_sle_data (*(volatile unsigned long *)ADC_GPADATA)

unsigned long ADC_GPACON, ADC_GPADATA; 
unsigned long ADC_0, ADC_1, ADC_2, ADC_3, ADC_4, ADC_5, ADC_6, ADC_7;
unsigned long ADC_DATA;

unsigned long adc_write_addr;
unsigned long adc_read_addr;


devfs_handle_t devfs_adc;

void adc0809_interrupt(int,void *,struct pt_regs *);

int  adc0809_open(struct inode *, struct file *);
int  adc0809_release(struct inode *, struct file *);
int  adc0809_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
ssize_t adc0809_read(struct file *, char * , size_t , loff_t *);
ssize_t adc0809_write(struct file *, const char *, size_t , loff_t *);

static struct file_operations adc0809_fops = {
        ioctl:          adc0809_ioctl,
        open:           adc0809_open,
        read:           adc0809_read,
        write:          adc0809_write,
        release:        adc0809_release,
};

unsigned long data;
unsigned long addr;

void adc0809_interrupt(int irq,void *d,struct pt_regs *regs)
{
  /*clear interrupt register for INT 2*/
  printk("********************** adc0809_interrupt *********************\n");
  SRCPND &= (~0x00000004);    //clear interrupt (bit2)
  INTPND = INTPND;
  //  EINTPEND &= (~0x00000004);  //bit2

  printk("interrupt process!\n");

  //udelay(100000);
  adc_read_addr = ioremap(0x10000020,4);

  /* read ad converter's result */
  data = (*(volatile unsigned long *) adc_read_addr);

}

/*
 * Open/close code for raw IO.
 */
int adc0809_open(struct inode *inode, struct file *filp)
{
  adc_read_addr = ADC_DATA;

  printk("open ok\n");
  return 0;
}

ssize_t adc0809_read(struct file *filp, char * buf,
                 size_t size, loff_t *offp)
{	
	char key;
	key = data;
	data = 0;
	if(key == 0){
	  //		printk("adc0809_read:have no data to read\n");
		return 0;
	}
	put_user(key,buf);
	
	return 1;
}

ssize_t adc0809_write(struct file *filp, const char *buf,
                  size_t size, loff_t *offp)
{
   char key;
   if (get_user(key, buf))
	       return -EFAULT;
   printk("adc0809_write:adc_write_addr=0x%x; key = %c\n",adc_write_addr,key);

	(*(volatile unsigned char *) adc_write_addr) = key;
   //put_user(key,buf);
   return 1;
}

//__ioremap
int adc0809_release(struct inode *inode, struct file *filp)
{
        printk("release ok\n");
        return 0;
}

/*
 * Deal with ioctls against the raw-device control interface, to bind
 * and unbind other raw devices.
 */
int adc0809_ioctl(struct inode *inode,
                  struct file *flip,
                  unsigned int command,
                  unsigned long arg)
{
      int err = 0;
      switch (command) { 
	case IOCTRL_ADC_0:
	  adc_write_addr = ADC_0;
	  //printk("adc0809_ioctl: adc addr: %x\n",adc_write_addr);
	  /* start collect and convert */
	  (*(volatile unsigned char *) adc_write_addr) = (char) arg;
	  return 0;
	case IOCTRL_ADC_1:
	  adc_write_addr = ADC_1;
	  (*(volatile unsigned char *) adc_write_addr) = (char) arg;
	  return 0;
	case IOCTRL_ADC_2:
	  adc_write_addr = ADC_2;
	  (*(volatile unsigned char *) adc_write_addr) = (char) arg;
	  return 0;
	case IOCTRL_ADC_3:
	  adc_write_addr = ADC_3;
	  (*(volatile unsigned char *) adc_write_addr) = (char) arg;
	  return 0;
	case IOCTRL_ADC_4:
	  adc_write_addr = ADC_4;
	  (*(volatile unsigned char *) adc_write_addr) = (char) arg;
	  return 0;
	case IOCTRL_ADC_5:
	  adc_write_addr = ADC_5;
	  (*(volatile unsigned char *) adc_write_addr) = (char) arg;
	  return 0;
	case IOCTRL_ADC_6:
	  adc_write_addr = ADC_6;
	  (*(volatile unsigned char *) adc_write_addr) = (char) arg;
	  return 0;
	case IOCTRL_ADC_7:
	  adc_write_addr = ADC_7;
	  (*(volatile unsigned char *) adc_write_addr) = (char) arg;
	  return 0;
	default:
	  err = -EINVAL;
      }
      // adc_read_addr = ADC_DATA;
      adc_write_addr = ADC_2;
	  (*(volatile unsigned char *) adc_write_addr) = (char) arg;
	    
      return err;		
}

int __init adc0809_init(void)
{
  static int result;
  unsigned long gpfup;
volatile unsigned int bankcon2;	
  
  printk("*********************adc0809_init**************\n");

  ADC_GPACON = (unsigned int)ioremap(0x56000000,4);
  ADC_GPADATA = ioremap(0x56000004,4);

  ADC_0 = ioremap(0x10000010,4);
  ADC_1 = ioremap(0x10002010,4);
  ADC_2 = ioremap(0x10004010,4);
  ADC_3 = ioremap(0x10006010,4);
  ADC_4 = ioremap(0x10008010,4);
  ADC_5 = ioremap(0x1000a010,4);
  ADC_6 = ioremap(0x1000c010,4);
  ADC_7 = ioremap(0x1000e010,4);
  
  ADC_DATA = ioremap(0x10000020,4);

  bankcon2=(volatile unsigned int)ioremap(0x4800000c,4);
  *(volatile unsigned int*)bankcon2 |= 3<<13;
  /* select NGCS2 */
  adc0809_sle |= 0x2000;
  adc0809_sle_data &= (~0x2000);

  adc_read_addr = ADC_DATA;
  
  /* set external irq rising edge */
  set_external_irq(IRQ_EINT2, EXT_RISING_EDGE, GPIO_PULLUP_DIS);

  /* set EINT2 pull up */
  gpfup = ioremap(0x56000058,4);
  (*(volatile unsigned long *)gpfup) = 0;

  printk("adc0809_irq : EXTERNAL_IRQ = %d\n",IRQ_EINT2);

  disable_irq(IRQ_EINT2);
  enable_irq(IRQ_EINT2);

  /* request irq */
  result=request_irq(IRQ_EINT2,&adc0809_interrupt,SA_INTERRUPT/*|SA_SHIRQ\*/,"adc0809",NULL);
  if (result)
    {
      printk("Can't get assigned irq %d,result=%d\n",IRQ_EINT2,result);
	     //              return result;
     }

  /* register char device */
  devfs_adc =
        devfs_register(NULL,"adc0809",DEVFS_FL_DEFAULT,
		       ADC0809_MAJOR, 0,
             S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
		       &adc0809_fops,NULL);
        return 0;
}

int __init adc0809_exit(void)
{
  free_irq(IRQ_EINT2, adc0809_interrupt);
  devfs_unregister(devfs_adc);
  return 0;
}

/*
__initcall(adc0809_init);
*/
module_init(adc0809_init);
module_exit(adc0809_exit);

