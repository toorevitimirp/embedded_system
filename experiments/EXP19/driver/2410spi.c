/*
 * User-space interface to the SPI bus on  the S3C2410(Thunder)
 *
 * (c) SAN People (Pty) Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>    /* error codes */

#include <linux/kernel.h>    /* printk() */
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/fs.h>     /* everything... */
#include <asm/semaphore.h>
#include <asm/arch/cpu_s3c2410.h>
#include <asm/hardware.h>

#include "2410spi.h"
#define DEVICE_NAME   "2410spi"

/*SPI registers*/
#define rSPCON0 (*(volatile unsigned char *)r_SPCON0)      /*SPI control Register*/
#define rSPSTA0 (*(volatile unsigned char *)r_SPSTA0)       /*SPI status Register*/
#define rSPPIN0 (*(volatile unsigned char *)r_SPPIN0)        /* SPI pin control Register*/
#define rSPPRE0 (*(volatile unsigned char *)r_SPPRE0)       /*SPI Baud Rate Prescaler Register */
#define rSPTDAT0 (*(volatile unsigned char *)r_SPTDAT0)  /*SPI Tx Data Register*/
#define rSPRDAT0 (*(volatile unsigned char *)r_SPRDAT0)  /*SPI Rx Data Register */

/*DMA registers*/
#define rDISRC1 (*(volatile unsigned char *)r_DISRC1)     /*DMA initial source Register */
#define rDISRCC1 (*(volatile unsigned char *)r_DISRCC1)     /*DMA initial source control Register */
#define rDIDST1 (*(volatile unsigned char *)r_DIDST1)     /*DMA initial destination Register */
#define rDIDSTC1 (*(volatile unsigned char *)r_DIDSTC1)/*DMA initial destination control Register */
#define rDCON1 (*(volatile unsigned char *)r_DCON1)     /*DMA control  Register */
#define rDSTAT1 (*(volatile unsigned char *)r_DSTAT1)     /*DMA status Register */
#define rDMASKTRIG1 (*(volatile unsigned char*)r_DMASKTRIG1) /*DMA mask trigger register*/

/*I/O registers*/
#define rGPECON (*(volatile unsigned char *)r_GPECON)    /*Configure the pins of port E*/
#define rGPEUP  (*(volatile unsigned char *)r_GPEUP)     /*Pull-up disable register for port E*/

static unsigned char *dbuf;

#if LINUX_VERSION_CODE < 0x020100
static struct semaphore sem = MUTEX;
#else
static DECLARE_MUTEX(sem);
#endif

/*define kernel virtual address space*/
unsigned long r_SPCON0,r_SPSTA0,r_SPPIN0,r_SPPRE0,r_SPTDAT0,r_SPRDAT0;
unsigned long r_DISRC1,r_DISRCC1,r_DIDST1,r_DIDSTC1,r_DCON1,r_DSTAT1,r_DMASKTRIG1;
unsigned long r_GPECON,r_GPEUP;

/*Remap an arbitrary physical address space into the kernel virtual address space.*/
int address_map(void)
{	
	/*SPI registers*/
	r_SPCON0 = _ioremap(0x59000000,4,0);
	r_SPSTA0  = _ioremap(0x59000004,4,0);
	r_SPPIN0   = _ioremap(0x59000008,4,0);
	r_SPPRE0  =  _ioremap(0x5900000C,4,0);
	r_SPTDAT0 = _ioremap(0x59000010,4,0);
	r_SPRDAT0 = _ioremap(0x59000014,4,0);

	/*DMA registers*/
	r_DISRC1 = _ioremap(0x4B000040,4,0);
	r_DISRCC1 = _ioremap(0x4B000044,4,0);
	r_DIDST1  = _ioremap(0x4B000048,4,0);
	r_DIDSTC1 = _ioremap(0x4B00004C,4,0);
	r_DCON1 = _ioremap(0x4B000050,4,0);
	r_DSTAT1 = _ioremap(0x4B000054,4,0);
	r_DMASKTRIG1 = ioremap(0x4B000060,4,0);

	/*I/O registers*/
	r_GPECON = _ioremap(0x56000060,4,0);
	r_GPEUP = _ioremap(0x56000068,4,0);
	return 0;
}

void
dma_interrupt(int irq,void *dev_id,struct pt_regs *regs)
{
   unsigned short dstat1 = rDSTAT1;
   if(dstat1&DSTAT1_CURR_TC)
	printk("DMA transfer is over"); 
   
}

/*Read to SPI bus*/
static ssize_t
spi_rd(struct file *filep,char * buffer,size_t length,loff_t * off)
{
	2410spi_dev *dev;
	
	down(&sem);

	dev = filep->private_data;	

	dbuf = (unsigned char *)buffer;

	/*set I/O port*/
	rGPECON |=0x8000c0;  /*GPE13:SPICLK0,GPE12:SPIMOSIO,GPE11:SPIMSSO0*/
	rGPEUP |= 0x3800;   /*Pull-up disable*/

	/*Check the status of Transfer Ready flag(REDY=1)*/
	while(1)
	{
	    if(rSPSTA0&SPSTA0_REDY)
	    {
		break;
	    }
	}
	
	/*set SPCON0 to configure properly the SPI module,
	   DMA mode ,SCK enable,  master mode , active high , format A, 
	   Tx auto garbage data mode 
	*/
	rSPCON0 =  0x5a;
	
	/*DMA is configured properly*/
	/*1.Base address of source data to transfer*/
	rDISRC1 = 0x59000014;
    
	/*2.LOC=1:the source is in the peripheral bus*/
	/*INC=0:the address is increased by its data size*/
	rDISRCC1 = 0x00000002;

	/*3.Base address of destination for the transfer*/
	rDIDST1 = dbuf;

	/*4.LOC=0:the source is in the system bus*/
	/*INC=0:the address is increased by its data size*/
	rDISTC1 = 0x00000000;

	/*5.DMD_HS=1:Handshake mode is selected;*/
	/*SYNC=1:DREQ and DACK are synchronized to HCLK*/
	/*INT=1:interrupt request is generated when all the transfer is done*/
	/*TSZ=0:a unit transfer is performed*/
	/*SERVMODE=0:Single service mode*/
	/*HWSRCSEL=011:SPI request DMA;SWHW_SEL=1:select the DMA source from hardware*/
	/*RELOAD=1;DSZ=00:Byte to be transferred;TC=length */
	rDCON1 = DCON1_DMD_HS|DCON1_SYNC|DCON1_INT|DCON1_TSZ|
		DCON1_SERVMODE|DCON1_HWSRCSEL|DCON1_SWHW_SEL|RELOAD|
		DCON1_DSZ|length;

	/*6.STOP=1;ON_OFF=1;SW_TRIG=0*/
	rDMASKTRIG1 = 0x006;

	
	

}

/*Write to SPI bus*/
static ssize_t
spi_wr(struct file *filep,char * buffer,size_t length,loff_t * off)
{
	int i = 0;
	2410spi_dev *dev;
	
	down(&sem);
	dev = filep->private_data;

	dbuf = (unsigned char*) buffer;

	/*set I/O port */
	rGPECON |= 0x0a8000c0; /*GPE13:SPICLK0,GPE12:SPIMOSI0,GPE11:SPIMISO0*/
	rGPEUP |= 0x3800;     /*Pull-up disable*/ 	

	/*Check the status of Transfer Ready flag(REDY=1)*/	
	while(1)
	{   
	    if(rSPSTA0&SPSTA0_REDY)
	    {
		break;
	    }
	}
	
	/*set SPCON0 to configure properly the SPI module,
	   DMA mode ,SCK enable,  master mode , active high , format A, 
	   receiving data normal mode 
	*/
	rSPCON0 =  0x58;

	/*DMA is configured properly*/
	/*1.Base address of source data to transfer*/
	rDISRC1 = dbuf;

	/*2.LOC=0:the source is in the system bus*/
	/*INC=0:the address is increased by its data size*/
	rDISRCC1 = 0x00000000;

	/*3.Base address of destination for the transfer*/
	rDIDST1 = 0x59000010;

	/*4.LOC=1:the source is in the peripheral bus*/
	/*INC=0:the address is increased by its data size*/
	rDISTC1 = 0x00000002;

	/*5.DMD_HS=1:Handshake mode is selected;SYNC=*/
	/*INT=1:interrupt request is generated when all the transfer is done*/
	/*TSZ=0:a unit transfer is performed*/
	/*SERVMODE=0:Single service mode*/
	/*HWSRCSEL=011:SPI request DMA;SWHW_SEL=1:select the DMA source from hardware*/
	/*RELOAD=1;DSZ=00:Byte to be transferred;TC=length */
	rDCON1 = DCON1_DMD_HS|DCON1_SYNC|DCON1_INT|DCON1_TSZ|
		DCON1_SERVMODE|DCON1_HWSRCSEL|DCON1_SWHW_SEL|RELOAD|
		DCON1_DSZ|length;

	/*6.STOP=1;ON_OFF=1;SW_TRIG=0*/
	rDMASKTRIG1 = 0x006;

	/*The SPI is configured as interrupt mode*/
	rSPCON0 = 0x20;	
	
}

static int
spi_ioctl (struct inode *inode,struct file *file,unsigned int cmd,unsigned long arg)
{
	int rc = 0;
	/*Configurable options could include :Speed ,Clock Polarity,Clock Phase*/
	switch (cmd)
	{
	    default:
		return -ENOIOCTLCMD;
	}//endswitch

	return rc;
}

/*Open the SPI device*/
static int 
spi_open(struct inode *inode,struct file *file)
{
	2410spi_dev *dev;
	if((dev = kmalloc (sizeof (2410spi_dev), GFP_KERNEL)) == NULL	)
	{
		return -ENOMEM;
	}

	//set default values
	dev->cpol = 0;
	dev->cpha = 0;
	file->private_data = dev;

	return 0;
}

/*Close the SPI device*/
static int 
spi_release (struct inode *inode,struct file *file)
{
	kfree (file->private_data);

	return 0;
	
}

struct  file_operation Fops = {
	    owner:THIS_MODULE,
	    read:spi_rd,
	    write:spi_wr,
	    ioctl:spi_ioctl,
	    open:spi_open,
	    release:spi_release
};

int init()
{
	if(request_irq
	    (INT_DMA1,dma_interrupt,SA_INTERRUPT,"DMA CHANNEL 1",NULL))
	{
	    printk();
	    return -EINVAL;
	}	

	/*set baud rates prescaler register*/
	rSPPRE0 = 2410SPI_BAUD;

	/*set SPCON0 to configure properly the SPI module,
	   DMA mode ,SCK enable,  master mode , active high , format A, 
	   receiving data normal mode 
	*/
	rSPCON0 =  0x58;

	
	return 0;
}

/*Install  the SPI /dev interface driver*/
static int __init
spi2410_init (void)
{ 
	int ret;

	if ((ret =
	     devfs_register_chrdev (2410SPI_MAJOR, DEVICE_NAME, &Fops) < 0))
	  {
		  printk ("%s device failed with %d\n",
			  "Sorry, registering the character", ret);
		  return ret;
	  }
	printk ("2410SPI device driver installed OK\n");
	return init ();
}

/*Remove   the SPI /dev interface driver*/
void __exit
spi2410_exit(void)
{
	int ret;
	
	free_irq(INT_DMA1,NULL);
 	 /*
	 * Unregister the device 
	 */
	if ((ret = devfs_unregister_chrdev (2410SPI_MAJOR, DEVICE_NAME)) < 0)
		printk ("Error in unregister_chrdev: %d\n", ret);
}


module_init(spi2410_init);

module_exit(spi2410_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rui zhuo");
MODULE_DESCRIPTION("SPI driver for S3C2410");
