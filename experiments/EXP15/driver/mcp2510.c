/*
 * User-space interface to the SPI bus on S3C2410
 *
 * (c) SAN People (Pty) Ltd
 *
 * Based on SPI driver by Rick Bronson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/config.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/iobuf.h>
#include <linux/highmem.h>
#include <linux/in.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <linux/tqueue.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#ifdef CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
#endif
#include "mcp2510.h"

#undef DEBUG_SPI

unsigned char * dbuf;
devfs_handle_t mcp2510_devfs_dir;

#define DEVICE_NAME  "MCP2510"
#define DEBUG_SPI

#define U8 unsigned char
#define U32 unsigned int

/*SPI registers*/
#define rSPCON0 (*(volatile unsigned long *)r_SPCON0)   /*SPI control Register*/
#define rSPSTA0 (*(volatile unsigned long *)r_SPSTA0)   /*SPI status Register*/
#define rSPPIN0 (*(volatile unsigned long *)r_SPPIN0)   /* SPI pin control
Register*/
#define rSPPRE0 (*(volatile unsigned long *)r_SPPRE0)  /*SPI Baud Rate Prescaler Register */
#define rSPTDAT0 (*(volatile unsigned long *)r_SPTDAT0) /*SPI Tx Data Register*/
#define rSPRDAT0 (*(volatile unsigned long *)r_SPRDAT0) /*SPI Rx Data Register*/

/*I/O registers*/
#define rGPECON (*(volatile unsigned long *)r_GPECON) /*Configure the pins of port E*/
#define rGPEUP  (*(volatile unsigned long *)r_GPEUP)  /*Pull-up disable register for port E*/
#define rGPGCON (*(volatile unsigned long *)r_GPGCON)    /*Configure the pins of port G*/
#define rGPGUP  (*(volatile unsigned long *)r_GPGUP)     /*Pull-up disable register for port G*/
#define rGPGDAT (*(volatile unsigned long *)r_GPGDAT)     /*The data register for port G*/

unsigned long r_SPCON0,r_SPSTA0,r_SPPIN0,r_SPPRE0,r_SPTDAT0,r_SPRDAT0;
unsigned long r_GPECON,r_GPEUP;
unsigned long r_GPGCON,r_GPGUP,r_GPGDAT;

unsigned char  TXdata[11];	 /*Transmit buffer*/
unsigned char  RXdata[11];	 /*Receive  buffer*/
unsigned char  flag;


/**********************************************************/
static void spi_poll_done(void);
void spi_tx_data(U8 data);
void Write_2510(U8 W_ADD, U8 W_DATA);
unsigned char Read_2510(U8 R_ADD);
void Reset_2510(void);
void Modify_2510(U8 M_ADD,U8 M_MASK,U8 M_DATA);
void SEND_TXB0(void);
void Init_MCP2510(void);
void Exit_MCP2510(void);
void Test(void);
void send_test_frame();
void Test_can_bus();
/**********************************************************/
//address_map
int address_map(void)
{
	//SPI registers
        r_SPCON0 = ioremap(0x59000000,4);
        r_SPSTA0 = ioremap(0x59000004,4);
        r_SPPIN0 = ioremap(0x59000008,4);
        r_SPPRE0 =  ioremap(0x5900000C,4);
        r_SPTDAT0 = ioremap(0x59000010,4);
        r_SPRDAT0 = ioremap(0x59000014,4);

	//I/O registers
        r_GPECON = ioremap(0x56000040,4);
        r_GPEUP  = ioremap(0x56000048,4);
		
	r_GPGCON = ioremap(0x56000060,4);
        r_GPGUP = ioremap(0x56000068,4);
        r_GPGDAT = ioremap(0x56000064,4);

	return 0;
}

/**********************************************************/
//To polling when SPI transfer is not finished
static void spi_poll_done(void)
{
	int nCount=0;
        
	while(!(rSPSTA0 & 0x01) )
	{
                nCount++;
                if(nCount>=5000)
		{
		   printk("SPI state poll failed\n");
                   break;
                }
   	}//endwhile	
}

/**********************************************************/
//Transmit data
void spi_tx_data(U8 data)
{	 
	spi_poll_done();
        rSPTDAT0 = data;	//transmit data
        spi_poll_done();	
}

/**********************************************************/
//Write to MCP2510 with ONE byte
void Write_2510(U8 W_ADD, U8 W_DATA)
{
	 
	rGPGDAT &=(~0x4000);	//Select the chip
 	udelay(100000);	
	
	spi_tx_data(CMD_WRITE);
	spi_tx_data(W_ADD);
	spi_tx_data(W_DATA);
	 
	rGPGDAT |=0x4000; 		//Unselect the chip
	
}

/**********************************************************/
//Read from MCP2510 with ONE byte
unsigned char Read_2510(U8 R_ADD)
{
	unsigned char buffer;
	
	rGPGDAT &=(~0x4000);       //Select the chip
	udelay(100000);

	spi_tx_data(CMD_READ);
	spi_tx_data(R_ADD);
	spi_tx_data(0xff);	
	buffer = rSPRDAT0;

	rGPGDAT |=0x4000;           //Unselect the chip

	return buffer;
}


/**********************************************************/
//Modify MCP2510
void Modify_2510(U8 M_ADD, U8 M_MASK, U8 M_DATA)
{	
	rGPGDAT &=(~0x4000);           //Select the chip
	udelay(100000);

	spi_tx_data(CMD_MODIFY);
	spi_tx_data(M_ADD);
	spi_tx_data(M_MASK);
	spi_tx_data(M_DATA);

	rGPGDAT |=0x4000;               //Unselect the chip
}


/**********************************************************/
//Send TXB0
void SEND_TXB0(void)
{
	rGPGDAT &=(~0x4000);           //Select the chip
	udelay(100000);
	
	spi_tx_data(CMD_RTS0);
	
	rGPGDAT |=0x4000;               //Unselect the chip
}

/**********************************************************/
//Reset 2510
void Reset_2510(void)
{
	unsigned char i;
	
	rGPGDAT &=(~0x4000);
	udelay(100000);

	spi_tx_data(CMD_RESET);

	rGPGDAT |=0x4000;

	for(i = 0; i < 128; i++); //Wait for OST
	

}

/**********************************************************/
//Initialize 2410 spi
void Init_SPI(void)

{
	int i;

       rSPPRE0 = 0xff;  //2410 SPI_BAUD; 
       rSPCON0 = 0x18;  //polling mode is used here!
       for(i = 0 ; i < 10 ; i++)
       {
    	    rSPTDAT0 = 0xff;
       }

       rGPECON |=   0x0a800000;
       rGPECON &= (~0x05400000);
       rGPEUP |= 0x3800;

       //GPG14----->CS
       rGPGCON |=   0x10000000;
       rGPGCON &= (~0x20000000);
       rGPGUP &= (~0x4000);
       rGPGDAT |=0x4000;                //Unselect the chip

}


/**********************************************************/
//Initialize mcp2510
void Init_MCP2510(void)
{
	unsigned char buffer;
	
	//Reset MCP2510
	Reset_2510();
	
	//Set to Configure Mode
	Modify_2510(CANCTRL, 0xe0, 0x80);
	Modify_2510(CANCTRL, 0x07, 0x00);

	//Wait into configure Mode
	while(1)
	{
		buffer = Read_2510(CANSTAT);
		printk("CANSTAT register is %x\n",buffer&0xe0);
		if((buffer&0xe0) == 0x80)
		{
			printk("In the Configure Mode\n");
			break;
		}
	}

	//Set physical layer configuration
	Write_2510(CNF1,0x07);
	Write_2510(CNF2,0x90);
	Write_2510(CNF3,0x02);

	//Set Interput , receive register 1 empty interrupt enable
	Write_2510(CANINTE, 0x02);

	//Configure Receive buffer 0 Mask and Filters
	//Receive buffer 0 will not be used
	Write_2510(RXM0SIDH,0Xff);    
	Write_2510(RXM0SIDL,0Xff);
	Write_2510(RXF0SIDH,0Xff);
	Write_2510(RXF0SIDL,0Xff);
	Write_2510(RXF1SIDH,0Xff);
	Write_2510(RXF1SIDL,0Xff);

	//Configure Receive buffer 1 Mask and Filters
	Write_2510(RXM1SIDH,0xff);   //RXB1 mathces all filters for Standad messages
	Write_2510(RXM1SIDL,0xe0);

//	Write_2510(RXM1SIDH,0x00);
//	Write_2510(RXM1SIDL,0x00);	

	//------------------Receives 1 message------------------------
	Write_2510(RXF2SIDH,0xa0);  
	//Initialize Filter 2 (will receive only b'1010 0000 000' message)
	Write_2510(RXF2SIDL,0x00);     
	//Make sure EXIDE bit (bit 3) is set correctly in filter also

	//------------------Receives 2 message------------------------
	Write_2510(RXF3SIDH,0xa1);
	Write_2510(RXF3SIDL,0x00);

	//------------------Receives 3 message------------------------
	Write_2510(RXF4SIDH,0xa2);
	Write_2510(RXF4SIDL,0x00);

	//------------------Receives 4 message------------------------
	Write_2510(RXF5SIDH,0xa3);
	Write_2510(RXF5SIDL,0x00);
			
	//Set RXB1CTRL Register
	Write_2510(RXB1CTRL, 0x22);

	//Set to Normal Mode
	Modify_2510(CANCTRL, 0xe0, 0x00);
	
  	//Wait into Normal Mode
   	while(1)
	{
	       buffer = Read_2510(CANSTAT);
	       printk("CANSTAT register is %x\n",buffer&0xe0);
	       if((buffer&0xe0) == 0x0)
	       {
		     printk("In the Normal Mode\n");
		     break;
	       }
	}
	
	printk("Init MCP2510 is over!\n");
	flag == 0x88;
}

/**********************************************************/
//Send data in the CAN bus
void can_data_send()
{
	unsigned char length,i;
	length = TXdata[2];
	Write_2510(TXB0SIDH,TXdata[0]);
	Write_2510(TXB0SIDL,TXdata[1]);
	Write_2510(TXB0DLC, TXdata[2]);
	
	rGPGDAT &=(~0x4000);	//Select the chip
	udelay(100000);
		
	spi_tx_data(CMD_WRITE);
	spi_tx_data(TXB0D0);
	for(i = 0; i<length; i++)
	{
		spi_tx_data(TXdata[i+3]);
	}
	
	rGPGDAT |=0x4000; 		//Unselect the chip

	SEND_TXB0();
#ifdef DEBUG_SPI
	printk("Transmit is over!\n");
#endif
	
}

/**********************************************************/
//Receive data in the CAN bus
void can_data_receive()
{	
	unsigned int length,i;
	length = Read_2510(RXB1DLC);

	rGPGDAT &=(~0x4000);	//Select the chip
	udelay(100000);
 	 	
	spi_tx_data(CMD_READ);
	spi_tx_data(RXB1D0);
	for(i = 0; i<length; i++)
	{
		spi_tx_data(0xff);
		RXdata[i] = rSPRDAT0;
		printk("Rxdata[%d] is %x\n",i,RXdata[i]);
	}
	
	rGPGDAT |=0x4000; 		//Unselect the chip

}

/**********************************************************/
//CAN interrupt program
void can_interrupt(int irq,void *d,struct pt_regs *regs)
{

	unsigned char buffer;

	buffer = Read_2510(CANSTAT);
	
	/*clear interrupt register for EINT21*/
	SRCPND &= (~0x00000020);    
   	INTPND = INTPND;
   	EINTPEND &= (~0x00200000);  

#ifdef DEBUG_SPI
        printk("Entered an interrupt! \n");
	printk("CANSTAT register is %x\n",buffer);
#endif

	if((buffer&0x0e) == 0x0e)
	{
#ifdef DEBUG_SPI
		printk("Recevie the sample data!\n");
#endif
		can_data_receive();  

		flag = 0xff;
	}
}


/**********************************************************/
/*********************Drive********************************/
/**********************************************************/
//Read  to MCP2510
static ssize_t mcp2510_rd(struct file *file, char *buf, size_t count, loff_t *offset)
{
	int ret = 0;
	int i = 0;

	dbuf = kmalloc(count*sizeof(unsigned char) , GFP_KERNEL);

	
	for(i = 0; i < count ; i++)		
	{
		dbuf[i] = RXdata[i];
	}
	
	copy_to_user(buf,dbuf,count);
	kfree(dbuf);

	return ret;
}

/**********************************************************/
// Write to MCP2510
static ssize_t mcp2510_wr(struct file *file, char *buf, size_t count, loff_t *offset)
{
	int ret = 0;
	int i = 0;

	dbuf = kmalloc(count*sizeof(unsigned char) , GFP_KERNEL);
     	
	copy_from_user(dbuf,buf,count);

	TXdata[0] = 0xb0;
	TXdata[1] = 0x00;
	TXdata[2] = count;

	for(i = 0; i < count; i++)
	{
		TXdata[i+3] = dbuf[i];
	}
#ifdef DEBUG_SPI1
	for(i = 0; i < 11; i++)
	{
			printk("Txdata[%d] is %x\n",i,TXdata[i]);
	}	
#endif
        //Transmit data to Bus	
        can_data_send();
	  
        kfree(dbuf);

        return ret;
}

/**********************************************************/
//Control the MCP2510 device
int mcp2510_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	
	switch(cmd) {
		default:
			return -ENOIOCTLCMD;
	}
}

/**********************************************************/
//Open the MCP2510 device
int mcp2510_open(struct inode *inode, struct file *file)
{
	return 0;
}

/**********************************************************/
//Close the MCP2510 device
static int mcp2510_close(struct inode *inode, struct file *file)
{
	return 0;
}

/**********************************************************/
static struct file_operations mcp2510_fops = {
	owner:		THIS_MODULE,
	llseek:		no_llseek,
	read:		mcp2510_rd,
	write:		mcp2510_wr,
	ioctl:		mcp2510_ioctl,
	open:		mcp2510_open,
	release:	mcp2510_close,
};

/**********************************************************/
// Install the MCP2510 driver
static int __init mcp2510_init(void)
{
	unsigned long gpfup;
	unsigned int result;

	//Set Interrupt 
	set_external_irq(IRQ_EINT21, EXT_FALLING_EDGE, GPIO_PULLUP_DIS);
	
	gpfup = ioremap(0x56000068,4);
	(*(volatile unsigned long *)gpfup) = 0;
		
	disable_irq(IRQ_EINT21);
        enable_irq(IRQ_EINT21);

	result = request_irq(IRQ_EINT21,&can_interrupt,SA_INTERRUPT,"can",NULL);
	if (result)
	{
	    printk("Can't get assigned irq %d,result=%d\n",IRQ_EINT7,result);
	    return result;
	}	

	//Register device
	mcp2510_devfs_dir = devfs_mk_dir(NULL,"CAN",NULL);		
	
	devfs_register(mcp2510_devfs_dir,DEVICE_NAME,DEVFS_FL_AUTO_DEVNUM,0,
			0,S_IFCHR|S_IRUGO|S_IWUGO,&mcp2510_fops,NULL);

	printk ("MCP2510 driver installed OK\n");

	
#ifdef DEBUG_SPI
printk("-------------->1\n");
printk("Initialize SPI\n");
#endif
	address_map();
	Init_SPI();

    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);
    spi_tx_data(0xff);

#ifdef DEBUG_SPI
printk("-------------->2\n");
printk("Initialize MCP2510\n");
	Init_MCP2510();
#endif

#ifdef DEBUG_SPI1
printk("-------------->2\n");
printk("Test Funciton 1\n");
	Test();
#endif

#ifdef DEBUG_SPI1
	printk("-------------->2\n");
	printk("Test Funciton 2\n");
	Test_can_bus();	 
#endif
		
	return 0;
}

/**********************************************************/
// Remove the MCP2510 driver
static void mcp2510_exit(void)
{	
	//Disable Interrupt
	disable_irq(IRQ_EINT21);
	free_irq(IRQ_EINT21, NULL);
	
	devfs_unregister(mcp2510_devfs_dir);
	
	printk ("MCP2510 driver uninstalled OK\n");
}


/**********************************************************/
//Test function(LAB 1)
void Test(void)
{
	unsigned char buffer[5];

	//Reset MCP2510
	Reset_2510();
	
	//Set to Configure Mode
   	Modify_2510(CANCTRL, 0xe0, 0x80);
   	Modify_2510(CANCTRL, 0x07, 0x00);

   	//Set CNF1
   	Write_2510(CNF1,0x07);
   	Write_2510(CNF2,0x10);
   	Write_2510(CNF3,0x02);

	Write_2510(RXB0CTRL,0x60);

	//Read registers
	buffer[0] = Read_2510(CANSTAT);
	buffer[1] = Read_2510(CNF1);
	buffer[2] = Read_2510(CNF2);
	buffer[3] = Read_2510(CNF3);
	buffer[4] = Read_2510(RXB0CTRL);
	
	printk("The CANSTAT register is  %x\n",buffer[0]);
	printk("The CNF1 register is  %x\n",buffer[1]);
	printk("The CNF2 register is  %x\n",buffer[2]);
	printk("The CNF3 register is  %x\n",buffer[3]);
	printk("The RXB0CTRL register is  %x\n",buffer[4]);
	
}


/**********************************************************/
//Test_CAN_BUS
void Test_can_bus()
{
	unsigned char buffer;
	int i;
	
	//Set Loop Mode
	Modify_2510(CANCTRL,0xe0,0x40);
	
	//Read the Mode
	buffer = Read_2510(CANSTAT);
	printk("The CANSTAT register is %x\n",buffer);

	//Transmit the sample data	
#ifdef DEBUG_SPI
	printk("Transmit the sample data\n");
#endif
	send_test_frame();
	
	//Receive the sample data	
	while(1)
	{
		if(flag == 0xff)
		{
			for(i = 0; i < 8; i++)
				printk("Rxdata[%d] is %c\n",i,RXdata[i]);
			break;
		}
	}
		
}

/**********************************************************/
//Send_test_frame
void send_test_frame()
{
	
	TXdata[0] =  0xa0;
	TXdata[1] =  0x00;

	TXdata[2] =  0x08;

	TXdata[3] =  'A' ;
	TXdata[4] =  'B' ;
	TXdata[5] =  'C' ;
	TXdata[6] =  'D' ;
	TXdata[7] =  'E' ;
	TXdata[8] =  'F' ;
	TXdata[9] =  'G' ;
	TXdata[10]=  'H' ;

	can_data_send();
	
}

module_init(mcp2510_init);

module_exit(mcp2510_exit);
