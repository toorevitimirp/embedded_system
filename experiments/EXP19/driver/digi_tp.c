/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Copyright (C) 2002, 2003 Motorola Semiconductors HK Ltd
 *
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <asm/uaccess.h>        /* get_user,copy_to_user */
#include <linux/miscdevice.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ptrace.h>
#include <linux/ioport.h>
#include <linux/in.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/init.h>
#include <asm/bitops.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <linux/tqueue.h>
#include <linux/wait.h>

#include <asm/irq.h>
#include <asm/arch/hardware.h>
#include <asm/arch/irqs.h>
//////////////////////////////////////////lyk add hhtech it///////////
//#include "digi.h"

/*
typedef unsigned int U32;       // unsigned 32 bit data 
#define MX1_ADS_BOAR 1
#define TPNL_INTR_MODE                          SA_INTERRUPT|SA_SHIRQ
#define TPNL_PEN_UPVALUE                        -999
#define EDGE_MIN_LIMIT  (100)
#define EDGE_MAX_LIMIT  (4000)

#define QT_IPAQ	

typedef struct {
#ifdef QT_IPAQ
	unsigned short pressure;
#else
	unsigned char pressure;
#endif
	unsigned short x;
	unsigned short y;
#ifdef QT_IPAQ
	unsigned short pad;
#endif
} ts_event_t;


#define  MAX_ID 	0x14

#ifdef QT_IPAQ
#define PENUP		0x0
#define PENDOWN		0xffff
#else
#define PENUP		0x0
#define PENDOWN		0xff
#endif
*/
////////////////////////////////////////

#ifdef MX21_ADS_BOARD
#include "mx2.h"
#endif
//<<<<<< Private Structure

//>>>>>> Private Structure


//<<<<<< Private Macro

#define MODULE_NAME "digi"

//#define DBMX_DEBUG 1
#ifdef DBMX_DEBUG
#  define TRACE(fmt, args...) \
	{ \
		printk("\n %s:%d:%s:",__FILE__, __LINE__,__FUNCTION__); \
		printk(fmt, ## args);\
	}
#else
#  define TRACE(fmt, args...)
#endif

#  define FAILED(fmt, args...) \
	{ \
		printk("\n %s:%d:%s:",__FILE__, __LINE__,__FUNCTION__); \
		printk(fmt, ## args);\
	}

#  define INFO(fmt, args...) \
	{ \
		printk("\n"); \
		printk(fmt, ## args);\
	}


//>>>>>> Private Macro
/*SPI registers*/
#define rSPCON0 (*(volatile unsigned long *)r_SPCON0)      /*SPI control Register*/
#define rSPSTA0 (*(volatile unsigned long *)r_SPSTA0)       /*SPI status Register*/
#define rSPPIN0 (*(volatile unsigned long *)r_SPPIN0)        /* SPI pin control Register*/
#define rSPPRE0 (*(volatile unsigned long *)r_SPPRE0)       /*SPI Baud Rate Prescaler Register */
#define rSPTDAT0 (*(volatile unsigned long *)r_SPTDAT0)  /*SPI Tx Data Register*/
#define rSPRDAT0 (*(volatile unsigned long *)r_SPRDAT0)  /*SPI Rx Data Register */

/*I/O registers*/
#define rGPECON (*(volatile unsigned long *)r_GPECON)    /*Configure the pins of port E*/
#define rGPEUP  (*(volatile unsigned long *)r_GPEUP)     /*Pull-up disable register for port E*/
//add by lyk hhtech
#define rGPGCON (*(volatile unsigned long *)r_GPGCON)    /*Configure the pins of port G*/
#define rGPGUP  (*(volatile unsigned long *)r_GPGUP)     /*Pull-up disable register for port G*/
#define rGPGDAT (*(volatile unsigned long *)r_GPGDAT)     /*The data register for port G*/
#define rGPFDAT (*(volatile unsigned long *)r_GPFDAT)     /*The data register for port G*/

unsigned long r_SPCON0,r_SPSTA0,r_SPPIN0,r_SPPRE0,r_SPTDAT0,r_SPRDAT0;
unsigned long r_GPECON,r_GPEUP;
unsigned long r_GPGCON,r_GPGUP,r_GPGDAT;
unsigned long r_GPFDAT;
//add by hhtech lyk
/*Remap an arbitrary physical address space into the kernel virtual address space.*/
int address_map(void)
{	
	/*
	//SPI registers
	r_SPCON0 = _ioremap(0x59000000,4,0);
	r_SPSTA0  = _ioremap(0x59000004,4,0);
	r_SPPIN0   = _ioremap(0x59000008,4,0);
	r_SPPRE0  =  _ioremap(0x5900000C,4,0);
	r_SPTDAT0 = _ioremap(0x59000010,4,0);
	r_SPRDAT0 = _ioremap(0x59000014,4,0);
	

	//I/O registers
	r_GPECON = _ioremap(0x56000040,4,0);
	r_GPEUP = _ioremap(0x56000048,4,0);
	//add by lyk hhtech
	r_GPGCON = _ioremap(0x56000060,4,0);
	r_GPGUP = _ioremap(0x56000068,4,0);
	r_GPGDAT = _ioremap(0x56000064,4,0);
	*/
	//SPI registers
	r_SPCON0 = ioremap(0x59000000,4);//,0);
	r_SPSTA0  = ioremap(0x59000004,4);//,0);
	r_SPPIN0   = ioremap(0x59000008,4);//,0);
	r_SPPRE0  =  ioremap(0x5900000C,4);//,0);
	r_SPTDAT0 = ioremap(0x59000010,4);//,0);
	r_SPRDAT0 = ioremap(0x59000014,4);//,0);


	//I/O registers
	r_GPECON = ioremap(0x56000040,4);//,0);
	r_GPEUP = ioremap(0x56000048,4);//,0);
	//add by lyk hhtech
	r_GPGCON = ioremap(0x56000060,4);//,0);
	r_GPGUP = ioremap(0x56000068,4);//,0);
	r_GPGDAT = ioremap(0x56000064,4);//,0);
	r_GPFDAT = ioremap(0x56000054,4);//,0);

	return 0;
}



//<<<<<Private Function Declearation
static int digi_open(struct inode * inode, struct file * filp);
static int digi_release(struct inode * inode, struct file * filp);
static int digi_fasync(int fd, struct file *filp, int mode);
static int digi_ioctl(struct inode * inode,struct file *filp,unsigned int cmd,unsigned long arg);
static ssize_t digi_read(struct file * filp, char * buf, size_t count, loff_t * l);
static unsigned int digi_poll(struct file * filp, struct poll_table_struct * wait);

static int check_device(struct inode *pInode);
static ts_event_t* get_data(void);
static void spi_tx_data(u16 data);
static int get_block(ts_event_t* * block, int size);
static void  digi_sam_callback(unsigned long data);

static void add_x_y(unsigned x, unsigned y, unsigned flag);
static int init_buf(void);
static void spi_flush_fifo(void);
//>>>>>Private Function Declearation

//<<<<<< Global Variable
static int	g_digi_major=0;
//static struct proc_dir_entry * g_proc_dir;
static devfs_handle_t g_devfs_handle;
static wait_queue_head_t digi_wait;
struct fasync_struct *ts_fasync=0;
static u16 	rptr, wptr;
static struct timer_list pen_timer;	
u8 pen_timer_status;	
spinlock_t pen_lock;
static struct tasklet_struct	digi_tasklet; 

struct file_operations g_digi_fops = 
                       {
		open:           digi_open,
		release:        digi_release,
		read:           digi_read,
		poll:           digi_poll,
		ioctl:          digi_ioctl,
		fasync:         digi_fasync,
                       };                       
//>>>>>> Global Variable


//<<<<<Body


#define NODATA()	(rptr==wptr)
/************************************/
//Misc functions for touch pannel
/*************************************/
static void touch_pan_enable(void)
{
#ifdef MX21_ADS_BOARD
	//set cspi1_ss0 to be low effective 
	//cspi1_ss0 --pd28, it connect with pen_cs_b
	_reg_GPIO_DR(3) &= (~0x10000000);
#endif
#ifdef MX1_ADS_BOARD
	rGPGDAT &=(~0x0004);
	//REG_PC_DR &= (~0x00008000);	
#endif	
}

static void touch_pan_disable(void)
{
#ifdef MX21_ADS_BOARD
	//set the cspi1_ss0 to be high 
	_reg_GPIO_DR(3) |= 0x10000000;
#endif
#ifdef MX1_ADS_BOARD
	//set the output [15] to be high 
	rGPGDAT |=0x0004;
	//REG_PC_DR |= 0x00008000;
#endif	
}

static void touch_pan_set_inter(void)
{
	int temp;
#ifdef MX21_ADS_BOARD
	printk("MX21_ADS_BOARD error\n");
	//pe10, uart3_cts pin connect to penIrq
	_reg_GPIO_GIUS(4) |= 0x00000400;
	_reg_GPIO_IMR(4)  &= ~(0x00000400);
	_reg_GPIO_DDIR(4) &= ~(0x00000400);
	temp = _reg_GPIO_SSR(4);
#endif
#ifdef MX1_ADS_BOARD
	set_external_irq(IRQ_EINT1,EXT_FALLING_EDGE, GPIO_PULLUP_DIS);

	//hhtech lyk add it
	//printk("MX1_ADS_BOARD beginning\n");
	//PenIrq is connected to PD31,//now changed to pb16 
	//first set bit31 of PortD GPIO GIUS_D
	//REG_PD_GIUS |= 0x80000000;
	//REG_PB_GIUS |= 0x00010000;
        /*
	*(volatile U32*)PTB_GIUS |= 0x1<<16; //GPC 14~17:GPIO function;
	//mask the Pen interrupt in PD[31]
	//printk("1\n");
	//REG_PB_IMR &= TOUCH_INT_MASK;	
        *(volatile U32*)PTB_IMR  &= ~(0x1<<16);  //all 14~17 interrupts masked	
	//printk("2\n");
	//REG_PB_PUEN |= 0x1<<16; //pull high
        *(volatile U32*)PTB_PUEN |= 0x1<<16; //pull high
	//set it to be input
	//printk("3\n");
//	REG_PB_DDIR &= 0xfffeffff
        *(volatile U32*)PTB_DDIR &= ~(0x1<<6); //14~17 input
	//printk("4\n");

//	REG_PB_ICR2 |= 0x00000001;
//	REG_PB_ICR2 &= ~0x00000002;
        *(volatile U32*)PTB_ICR2 |= 0x00000001; //PC16,PC17,negtive edge interru
	//printk("5\n");
	//Karen modify 8/05
	//printk("io addr=%8x\n",IO_ADDRESS(0x0021C124));
	temp = REG_PB_SSR;
	*/
	//printk("6\n");
#endif	
	
}

static void touch_pan_enable_inter(void)
{
#ifdef MX21_ADS_BOARD
	_reg_GPIO_IMR(4) |= 0x00000400;	
#endif
#ifdef MX1_ADS_BOARD
	//unmask the Pen interrupt in PD[31]
	//REG_PD_IMR |= ~TOUCH_INT_MASK;
        //*(volatile U32*)PTB_IMR |= 0x1<<16; // enable pin 14~17
        enable_irq(IRQ_EINT1);

#endif	
}

static void touch_pan_disable_inter(void)
{
#ifdef MX21_ADS_BOARD
	_reg_GPIO_IMR(4) &= ~(0x00000400);
#endif
#ifdef MX1_ADS_BOARD
	//mask the Pen interrupt in PD[31]
	//REG_PD_IMR &= TOUCH_INT_MASK;	
        //*(volatile U32*)PTB_IMR  &= ~(0x1<<16);    //all 14~17 interrupts masked
	disable_irq(IRQ_EINT1);
#endif	

}

static void touch_pan_clear_inter(void)
{
#ifdef MX21_ADS_BOARD
	_reg_GPIO_ISR(4) = 0x00000400;
#endif
#ifdef MX1_ADS_BOARD
	//REG_PD_ISR = ~TOUCH_INT_MASK;
 	//       *(volatile U32*)PTB_ISR  |= 0x1<<16;    //clear all 14~17 interrupts(wri
	SRCPND &= (~0x0000000c);
        INTPND = INTPND;
        EINTPEND &= (~0x00800000);

#endif	
}

/*static void touch_pan_set_pos_inter(void)
{
#ifdef MX21_ADS_BOARD
	_reg_GPIO_ICR1(4) &= 0xffcfffff;
#endif
#ifdef MX1_ADS_BOARD
	//here set it to be positive edge sensitive
	//REG_PD_ICR2 &= 0x3fffffff;
        *(volatile U32*)PTB_ICR2 &= 0xfffffffc; //PC16,negtive edge interru
#endif	
}*/

//here set it to be Negative level sensitive
/*static void touch_pan_set_neg_inter(void)
{
#ifdef MX21_ADS_BOARD
	_reg_GPIO_ICR1(4) &= 0xffcfffff;
	_reg_GPIO_ICR1(4) |= 0x00100000;
#endif
#ifdef MX1_ADS_BOARD
	//REG_PD_ICR2 &= 0x7fffffff;
	//REG_PD_ICR2 |= 0x40000000; //to be 01
	//set to 01
        *(volatile U32*)PTB_ICR2 &= 0xfffffffd; //PC16,negtive edge interru
        *(volatile U32*)PTB_ICR2 |= 0x00000001; //PC16,negtive edge interru

#endif
}*/

//here check if it is positive level sensitive
/*
static u8 touch_pan_check_int_pol(void)
{
#ifdef MX21_ADS_BOARD
	if((_reg_GPIO_ICR1(4) & 0x00300000) == 0x00100000)
	return 0;
	else
	return 1;
#endif
#ifdef MX1_ADS_BOARD
	//lyk hhtech changed it it
	if((REG_PB_ICR2 & 0x00000003) == TOUCH_INT_NEG_POLARITY)
	//return 0;
	return 1;
	else
	//return 1;
	return 0;
#endif
}*/

void touch_pan_init(void)//chip select for ADC
{
#ifdef MX21_ADS_BOARD
	//cspi1_ss0 --pd28, it connect with pen_cs_b
	_reg_GPIO_GIUS(3) |= 0x10000000;
	_reg_GPIO_OCR2(3) |= 0x03000000;
	_reg_GPIO_DR(3) |= 0x10000000;	
	_reg_GPIO_DDIR(3) |= 0x10000000;
#endif
#ifdef MX1_ADS_BOARD
	rGPGCON |=   0x00000010;
	rGPGCON &= (~0x00000020);
	rGPGUP &= (~0x0004);
	rGPGDAT |=0x0004;		
	/*
	//Here should also configure the Touch pannel select pin
	//Port C 15, it should be configured to be GPIO function
	//first set GIUS to be 1 to select GPIO signal
	REG_PC_GIUS |= 0x00008000;
	//then set the OCR1 to be 11 to use data register
	REG_PC_OCR1 |= 0xc0000000;
	//SPI1_SS to be output bit 15
	REG_PC_DDIR |= 0x00008000;
	//set PUEN to be 1 to set the pin is pulled high
	REG_PC_PUEN |= 0x00008000;
	//set the output [15] to be high disable
	REG_PC_DR |= 0x00008000;	
	*/
#endif
}

//read data from Touch Pannel
void touch_pan_read_dev(u32* x, u32* y)
{
	u32 x_upper, x_lower, y_upper, y_lower;
	u32 temp;
#ifdef MX21_ADS_BOARD
	temp = _reg_GPIO_ISR(4);
#endif
#ifdef MX1_ADS_BOARD
///	temp = REG_PB_ISR;
#endif
	//hhtech lyk add it
	//printk("TP read dev 1\n");
	//touch_pan_disable_inter();
	touch_pan_enable();
	//printk("TP read dev 2\n");
#ifdef MX21_ADS_BOARD	
	spi_tx_data(0xd0);
	x_upper = _reg_CSPI_RXDATAREG(1);
	spi_tx_data(0x0);
	x_upper = _reg_CSPI_RXDATAREG(1);
	spi_tx_data(0x90);
	x_lower = _reg_CSPI_RXDATAREG(1);
	spi_tx_data(0x0);
	y_upper = _reg_CSPI_RXDATAREG(1);
	spi_tx_data(0x0);
	y_lower = _reg_CSPI_RXDATAREG(1);
#endif
#ifdef MX1_ADS_BOARD
	/*spi_tx_data(0xD0);
	x_upper = REG_SPI_RXDATA;// this should be the dummy data
	spi_tx_data(0x00);
	x_upper = REG_SPI_RXDATA;
	spi_tx_data(0x90);
	x_lower = REG_SPI_RXDATA;
	spi_tx_data(0x00);
	y_upper = REG_SPI_RXDATA;
	spi_tx_data(0x00);
	y_lower = REG_SPI_RXDATA;
	*/
	spi_tx_data(0xD0);
	x_upper = rSPRDAT0;//REG_SPI_RXDATA;// this should be the dummy data
	spi_tx_data(0x00);
	x_upper =  rSPRDAT0;//REG_SPI_RXDATA;
	spi_tx_data(0x90);
	x_lower = rSPRDAT0;//REG_SPI_RXDATA;
	spi_tx_data(0x00);
	y_upper = rSPRDAT0;//REG_SPI_RXDATA;
	spi_tx_data(0x00);
	y_lower = rSPRDAT0;//REG_SPI_RXDATA;

#endif	
	
    *x=( ((x_upper<<5) & 0xFE0) | ((x_lower>>3) & 0x1F) );
    *y=( ((y_upper<<5) & 0xFE0) | ((y_lower>>3) & 0x1F) );
	touch_pan_disable();
#ifdef MX21_ADS_BOARD
	_reg_CSPI_CONTROLREG(1) &= ~0x00000400; //disable spi
	_reg_GPIO_ISR(4) = temp;
#endif
	//printk("TP read dev 3\n");

#ifdef MX1_ADS_BOARD
	//REG_SPI_CONTRL &= (~SPI_EN_BIT);//disable SPI
//	REG_PB_ISR = temp;
#endif
	//printk("TP read dev 4\n");
	//touch_pan_enable_inter();
	//printk("TP read dev 5\n");
	
}
void touch_pan_read_data(u32* x,u32* y)
{
#define NUM_OF_SAMPLE  (2)
#define MAX_POS_DIFF   (2)
    u32 xPos[NUM_OF_SAMPLE], yPos[NUM_OF_SAMPLE];

	touch_pan_read_dev(&xPos[0], &yPos[0]);
    if ((xPos[0]<100)||(xPos[0]>5000) ||
        (yPos[0]<100)||(yPos[0]>5000))
    {
        *x = TPNL_PEN_UPVALUE;
        *y = TPNL_PEN_UPVALUE;
        return;
    }
	touch_pan_read_dev(&xPos[1], &yPos[1]);
    if ((xPos[1]<100)||(xPos[1]>5000) ||
        (yPos[1]<100)||(yPos[1]>5000))
    {
        *x = TPNL_PEN_UPVALUE;
        *y = TPNL_PEN_UPVALUE;
        return;
    }

/*	touch_pan_read_dev(&xPos[2], &yPos[2]);
    if ((xPos[2]<100)||(xPos[2]>5000) ||
        (yPos[2]<100)||(yPos[2]>5000))
    {
        *x = TPNL_PEN_UPVALUE;
        *y = TPNL_PEN_UPVALUE;
        return;
    }
	touch_pan_read_dev(&xPos[3], &yPos[3]);
    if ((xPos[3]<100)||(xPos[3]>5000) ||
        (yPos[3]<100)||(yPos[3]>5000))
    {
        *x = TPNL_PEN_UPVALUE;
        *y = TPNL_PEN_UPVALUE;
        return;
    }
	*x = (xPos[0] + xPos[1] + xPos[2] + xPos[3])>>2;
	*y = (yPos[0] + yPos[1] + yPos[2] + yPos[3])>>2;*/
	
	*x = (xPos[0] + xPos[1])>>1;
	*y = (yPos[0] + yPos[1])>>1;

    return;
}

/******************************************************************************
 * Function Name: digi_isr
 *
 * Input: 		filp	:
 * 			wait	: 
 * Value Returned:
 *
 * Description: support touch pannel interrupt
 *
 *****************************************************************************/
static void 
digi_isr(int irq, void *dev_id, struct pt_regs *regs)
{
	//judge if it is the pen interrupt
#ifdef MX1_ADS_BOARD
	//if((REG_PB_ISR & (~TOUCH_INT_MASK))!=0)
	//hhtech lyk edit it
	//printk("TP:SRCPND = %08x,INTPND=%08x,EINTPEND=%08x,rGPFDAT=%08x\n",SRCPND,INTPND,EINTPEND,rGPFDAT);
	//SRCPND &= 0;//(~0x00000002);
        //INTPND = INTPND;
        //EINTPEND &= 0;//(~0x00000002);
	//touch_pan_clear_inter();
	//touch_pan_disable_inter();
	//r_GPGCON &= (~0x00000030);	
	if((rGPFDAT & 0x00000002)==0)
	{
		//printk("digi interrupt\n");
		touch_pan_clear_inter(); 
		touch_pan_disable_inter();
		tasklet_schedule(&digi_tasklet);
	}	
#endif	
#ifdef MX21_ADS_BOARD
	if((_reg_GPIO_ISR(4) & 0x00000400)!=0)
	{
		touch_pan_clear_inter(); 
		tasklet_schedule(&digi_tasklet);
	}	
#endif
	return;
}
static void digi_tasklet_action(unsigned long data)
{
	//lyk	
	u32	newX;
	u32  newY;
	u32  maxDelay;
	newX = TPNL_PEN_UPVALUE;
	maxDelay = 0;
//	save_flags(touchflags);
//	cli();
	
	//check the interrupt polarity to see it is pen up or down.
//	printk("TP: enter interrupt\n");
	//hhtech lyk deleted it
	//if(touch_pan_check_int_pol())//it is pen down
	{
		//printk("TP: pen down\n");
#ifdef MX21_ADS_BOARD
		if((_reg_GPIO_SSR(4) & 0x00000400) == 0)//judge if is pen down
		{
			return;			
		}
#endif
#ifdef MX1_ADS_BOARD
		//printk("pen down REG_PB_SSR=%x\n",REG_PB_SSR);
		//if((REG_PD_SSR & 0x80000000) == 0)//judge if is pen down
//		printk("judge if it is pen down\n");
		if( (rGPFDAT&0x00000002) != 0)//it is pen up
		{
			//printk("it is pen up here\n");
			touch_pan_enable_inter();
			/*if(pen_timer_status==1){
                                add_x_y(0,0,PENUP);
                                pen_timer_status=0;
                        }*/
			return;			
		}
#endif		
		//printk("it is pen down\n");
		//check if the pen sampling timer is in use
		//printk("pen_timer_status=%d\n",pen_timer_status);	
		//if(pen_timer_status!=0)//in use
		//{
			//then just change the interrupt polarity to be pen up
			//change to be 0x01
		//	touch_pan_set_neg_inter();
		//	return;
		//}
		//else //no sampling timer
		{
			//read data with MAX delay from ADC
			//printk("TP:touch_pan_read_data\n");
			touch_pan_read_data(&newX, &newY);

			//while((newX == TPNL_PEN_UPVALUE)&&(maxDelay <= 100))
			while((newX == TPNL_PEN_UPVALUE)&&(maxDelay <= 3))
			{
				//printk("maxdelay=%d\n",maxDelay);
				touch_pan_read_data(&newX, &newY);
				maxDelay++;
			}
			if(newX == TPNL_PEN_UPVALUE)
			{
				//clear the interrupt bit
				/*if(pen_timer_status==1){
					add_x_y(0,0,PENUP);
					pen_timer_status=0;
				}*/
				//add_x_y(0,0,PENUP);
				touch_pan_enable_inter();
				//printk("TouchPanIsr: error always PEN_UP \n");
				return;
			}
			//printk("TP:before add\n");
			TRACE("TouchPanIsr:get pendata x= %d, y=%d \n",newX,newY);
			add_x_y(newX, newY, PENDOWN);
			//printk("TP:After add_x_y:x=%d,y=%d\n",newX,newY);
			wake_up_interruptible(&digi_wait);
	
			/*if(ts_fasync){
				printk("TP:kill_fasync do what here?\n");
				kill_fasync(&ts_fasync, SIGIO, POLL_IN);
			}*/


			//then just change the interrupt polarity to be pen up
			//to be 0x01
			
			//touch_pan_set_neg_inter();

			spin_lock_irq(&pen_lock);
			//pen_timer.expires = jiffies + HZ/50 + 2*HZ/100;
			pen_timer.expires = jiffies + HZ/100;
			add_timer(&pen_timer);
			pen_timer_status = 1;
			spin_unlock_irq (&pen_lock);
			//printk("TP this time pen down over\n");
			return;
		}
	}
	//hhtech lyk deleted it
	//else//pen up interrupt
	{
#ifdef MX21_ADS_BOARD
		if((_reg_GPIO_SSR(4) & 0x00000400) != 0) //value is high
		{
			return;			
		}
#endif
#ifdef MX1_ADS_BOARD
		//printk("pen up REG_PB_SSR=%08x\n",REG_PB_SSR);
		//if((REG_PD_SSR & 0x80000000) != 0) //value is high
		/*if((REG_PB_SSR & 0x00010000) != 0) //value is high
		{
			return;			
		}*/
#endif
		//check if the pen sampling timer is in usei
		printk("pen_timer_status=%08x\n",pen_timer_status);
		if(pen_timer_status!=0)//in use
		{
			//disable pen down interrupt
			//get Pen up
			TRACE("TouchPanIsr:get pen up 0,0,penup \n");
			add_x_y(0, 0, PENUP);

		// if there is a pen up, shall tell apps to handle the event, and it shall
		// be data there
			if (!NODATA())
			{
				wake_up_interruptible(&digi_wait);
				if(ts_fasync)
				kill_fasync(&ts_fasync, SIGIO, POLL_IN);
			}

			//release timer
			spin_lock_irq(&pen_lock);
			pen_timer_status = 0;
			del_timer(&pen_timer);
			spin_unlock_irq(&pen_lock);
			
			//touch_pan_set_pos_inter();//set polarity to detach pen down
		}
		else //no timer
		{
			//////////////////////////////////////
			//lyk hhtech added it
			//touch_pan_set_pos_inter();
			////////////////////////////////////
			printk("TP:do nothing here\n");
			FAILED("TouchPanIsr :pen up, no timer \n");
		}
		printk("TP: all over here\n");
		return;
		
	}
	
}

/******************************************************************************
 * Function Name: digi_sam_callback
 *
 * Input: 		:
 * * Value Returned:
 *
 * Description: spi sampling timer call back
 *
 *****************************************************************************/
static void digi_sam_callback(unsigned long data)
{
	u32 newX,newY;
	u32  maxDelay;
	maxDelay = 0;
	newX = TPNL_PEN_UPVALUE;
//	save_flags(samflags);
//	cli();
//	printk("TP: digi_sam_callback function is called \n");
	//if(pen_timer_status !=0)
	mod_timer(&pen_timer, jiffies + HZ/100);
		//mod_timer(&pen_timer, jiffies + HZ/50 + 2*HZ/100);
#ifdef MX21_ADS_BOARD
	if((_reg_GPIO_SSR(4) & 0x00000400) == 0)//judge if is pen down
#endif
#ifdef MX1_ADS_BOARD
	//if((REG_PD_SSR & 0x80000000) == 0)//judge if is pen down
	//if((REG_PB_SSR & 0x00010000) == 0)//judge if is pen down
	//if((REG_PB_SSR & 0x00010000) != 0)//judge if is pen down
	//if((SRCPND & 0x00000002) == 0)//it is pen up here
	if((rGPFDAT&0x00000002) != 0)//it is pen up here
#endif	
	{
//		printk("pen up in call back\n");
		//if(pen_timer_status!=0)//in use
		{
add_x_y_line:			//get Pen up
			add_x_y(0, 0, PENUP);
			// if there is a pen up, shall tell apps to handle the event, and it shall be data there
			if (!NODATA())
			{
				wake_up_interruptible(&digi_wait);
				if(ts_fasync)
				kill_fasync(&ts_fasync, SIGIO, POLL_IN);
			}
			
			//release timer
			spin_lock_irq(&pen_lock);
			pen_timer_status = 0;
			del_timer(&pen_timer);
			spin_unlock_irq(&pen_lock);

			//touch_pan_set_pos_inter();//set polarity to detach pen down
			//hhtech added by lyk
			touch_pan_enable_inter();	
			touch_pan_clear_inter();
			
		}
		return;
	}else
	//if(!touch_pan_check_int_pol())//positive,it is pen down
	{
		//read data with MAX delay from ADC
//		printk("pen down in call back\n");
		while((newX == TPNL_PEN_UPVALUE)&&(maxDelay <= 100))
		{
			touch_pan_read_data(&newX, &newY);
			maxDelay++;
		}
		if(newX == TPNL_PEN_UPVALUE)
		{
//			printk("SpiSamCallback: error always PEN_UP \n");
			goto add_x_y_line;
			return;
		}
		//add element into buffer
		add_x_y(newX, newY, PENDOWN);
		//printk("add_x_y in call back,x=%d,y=%d\n",newX,newY);
		wake_up_interruptible(&digi_wait);
		
		if(ts_fasync)
			kill_fasync(&ts_fasync, SIGIO, POLL_IN);
		return;	
	}
	/*
	else
	{
		FAILED(" error to call me \n");
		return;
	}*/

}
/************************************************
Misc functions for SPI 
*************************************************/
void spi_init(void)
{
	int i;
#ifdef MX21_ADS_BOARD
	int datarate;
	int cs;
	datarate = 0x0111;//512
//	datarate = 0x1000;//1024
	cs = 0x0;//ss0
//	cs = 0x1;//ss1	
	//cspi1_mosi--pd31 output
	//cspi1_miso--pd30 input
	//cspi1_sclk--pd29 output
	//cspi1_ss0 --pd28,output it connect with pen_cs_b
	_reg_GPIO_GIUS(3) &= 0x0fffffff;
	_reg_GPIO_GPR(3) &= 0x0fffffff;
	_reg_GPIO_DDIR(3) |= 0xb0000000;
	_reg_GPIO_DDIR(3) &= 0xbfffffff;
	
	//reset spi1
	_reg_CSPI_RESETREG(1) = 0x00000001;
	sleep(2);//wait
	//bit19,18 CS[1:0], bit 17~14 datarate[3:0]
	_reg_CSPI_CONTROLREG(1) |= (cs<<18);
	_reg_CSPI_CONTROLREG(1) |= (datarate<<14);
	_reg_CSPI_CONTROLREG(1) |= 0x00000c07;//ss low active, 8 bit transfer,master mode
#endif
#ifdef MX1_ADS_BOARD
	//lyk hhtech edited it.
	rSPPRE0 = 0xff;//2410SPI_BAUD; who know how is right here?
	rSPCON0 =  0x18;//58 is dma mode, polling mode is used here!
	for(i=0;i<10;i++)
		rSPTDAT0 = 0xff;	
	rGPECON |=   0x0a800000;
	rGPECON &= (~0x05400000); 
	rGPEUP |= 0x3800;	
	/*
	//Port C 13,14, 15, 16, 17
	//first clear bit in GIUS register, then clear bit in GPR register
	REG_PC_GIUS &= 0xfffc1fff;//0x0021C220
	REG_PC_GPR &= 0xfffc1fff;//0x0021C238
	//SPI1_SCLK to be output bit 14
	//SPI1_SS to be output bit 15
	//SPI_MISO to be input bit 16
	//SPI_MOSI to be output bit 17
	REG_PC_DDIR |= 0x0002c000;//0x0021301C
	REG_PC_DDIR &= 0xfffeffff;//0x0021C200
	//Reset SPI 1 
	REG_SPI_SOFTRESET = 0x00000001;//0x0021301C
	for(i=0;i<4000;i++);//loop
	REG_SPI_CONTRL = 0x0000E607;//data rate be divide by 512,bit_count be 8 bit transfer 0x00213008
	*/
#endif	 
}
//To polling when Spi transfer is not finished
static void spi_poll_done(void)
{
	int nCount=0;
#ifdef MX21_ADS_BOARD
	//here should poll the IRQ bit in IRQ reg to see 
	while(!(_reg_CSPI_INTREG(1) & SPI_TE_INT_BIT));//transfer FIFO is empty
    /* Reset SPI IRQ bit */
	_reg_CSPI_INTREG(1) &= SPI_TE_INT_BIT;
	//wait until XCH bit is clear, hence exchange is complete
	while(!(_reg_CSPI_CONTROLREG(1) & SPI_XCH_BIT));
#endif
#ifdef MX1_ADS_BOARD
	//here should poll the IRQ bit in IRQ reg to see 
	//hhtech lyk add it
	while(!(rSPSTA0 & 0x01)	){
		nCount++;
		if(nCount>=5000){
			printk("TP spi state poll failed\n");
			break;
		}
	}
	
/*	while(!(REG_SPI_INTER & SPI_TE_INT)){//transfer FIFO is empty
		nCount++;
		if(nCount>=5000){
			//printk("TP::::::::::::::::::::::::::::::::::::::::spi_poll_done:REG_SPI_INTER:nCount=%d\n",nCount);
			break;
		}
	}*/
    /* Reset SPI IRQ bit */
	/*REG_SPI_INTER &= SPI_TE_INT;
	//wait until XCH bit is clear, hence exchange is complete
	nCount=0;
	//hhtech lyk add it
	while(!(REG_SPI_CONTRL & SPI_XCH_BIT)){
		nCount++;
                if(nCount>=5000){
                        //printk("TP:;;;;;;;;;;;;;;;;;;;;;;;;;;;;spi_poll_done:REG_SPI_INTER:nCount=%d\n",nCount);
                        break;
                }
	}*/
#endif	
}

static void spi_tx_data(u16 data)
{
    /* ensure data will not be transmitted while writing to SPI */
#ifdef MX21_ADS_BOARD    
   	_reg_CSPI_CONTROLREG(1) &= SPI_XCH_MASK;
	
    _reg_CSPI_CONTROLREG(1) |= SPI_EN_BIT;
    
    _reg_CSPI_TXDATAREG(1)  = data;//transfer data

    /* exchange data */
    _reg_CSPI_CONTROLREG(1) |= SPI_XCH_BIT;
    spi_poll_done();
   	_reg_CSPI_CONTROLREG(1) &= SPI_XCH_MASK;
#endif
#ifdef MX1_ADS_BOARD
	//lyk hhtech add it
	//REG_SPI_CONTRL &= 0xffff3fff;
	spi_poll_done();
	rSPTDAT0=data; 
	spi_poll_done();
	/*  	
	REG_SPI_CONTRL &= SPI_XCH_MASK;
	
    	REG_SPI_CONTRL |= SPI_EN_BIT;
    
    	REG_SPI_TXDATA  = data;//transfer data

    	// exchange data 
    	REG_SPI_CONTRL |= SPI_XCH_BIT;
    	spi_poll_done();
   	REG_SPI_CONTRL &= SPI_XCH_MASK;
	*/
#endif   	
}

/******************************************************************************
 * Function Name: check_device
 *
 * Input: 		inode	:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: verify if the inode is a correct inode
 *
 *****************************************************************************/
static int check_device(struct inode *pInode)
{
	int minor;
	//lyk hhtech modified it
	kdev_t dev = pInode->i_rdev;
	//return 0;	
	if( MAJOR(dev) != g_digi_major)
	{
		printk("DIGI: check_device bad major = %d,g_digi_major=%d\n",MAJOR(dev),g_digi_major );
		return -1;
	}
	minor = MINOR(dev);
	
	if ( minor < MAX_ID )
		return minor;
	else
	{       
		printk("DIGI: check_device bad minor = %d\n",minor );
		return -1;
	}
}

/**************************************************
Misc functions for buffer
***************************************************/
/* local buffer for store data
 * a new feature is read block, assume the buffer is a continuous buffer
 * so, if there is enough data, read data from current position to 
 * buffer end, then next time, from buffer head to data end. -- the buffer
 * is a loop buffer.
 */
ts_event_t * buffer;
#define BUFLEN		250
#define BUFSIZE 	((BUFLEN+1)*sizeof(ts_event_t))
#define NEXTI(i)	{i=((i==BUFLEN-1)?0:i+1);}
#define GETNEXTI(i)	((i==BUFLEN-1)?0:i+1)
#define BLKEND(i)	((i)==0)

#if 1
#define DSIZE()		((wptr<rptr)?(BUFLEN-rptr):(wptr-rptr))
#else
#define DSIZE()		((wptr<rptr)?(BUFLEN-rptr+wptr):(wptr-rptr))
#endif


void spi_flush_fifo(void)
{
	u32 i,j;
	for(i=0;i<8;i++)
	{
#ifdef MX21_ADS_BOARD
		if(_reg_CSPI_INTREG(1)&SPI_RR_INT_BIT)
		j = _reg_CSPI_RXDATAREG(1);
#endif
#ifdef MX1_ADS_BOARD
		//if(REG_SPI_INTER&SPI_RR_INT)
		//j = REG_SPI_RXDATA;
#endif
	}
}
/******************************************************************************
 * Function Name: init_buf 
 *
 * Input: 		void	:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: init the loop buffer for store data read out from spi FIFO
 * 	and shall be copied to user
 *
 *****************************************************************************/
static int init_buf(void)
{
	buffer = (ts_event_t*) vmalloc(BUFSIZE);	
	if(!buffer){
		printk(KERN_ERR"no enough kernel memory for spi data buffer\n");
		return -1;
	}

	rptr = wptr = 0;
	return 1;
}

/******************************************************************************
 * Function Name: add_x_y
 *
 * Input: 		i	:
 * Value Returned:	void	: 
 *
 * Description: add pen data to buffer
 *
 *****************************************************************************/
static void add_x_y(unsigned x, unsigned y, unsigned flag)
{
	
	if (GETNEXTI(wptr) == rptr)
		return;
	buffer[wptr].x = x;
	buffer[wptr].y = y;

	buffer[wptr].pressure = flag;

	NEXTI(wptr);	// goto next wptr
}

/******************************************************************************
 * Function Name: get_block
 *
 * Input: 		block:
 				size:
 * Value Returned:	int	: count of data size
 *
 * Description: read a block of 'touch' data from buffer, read count shall less
 *	than size,but shall be as more as possible. the data shall not really copy
 *  to upper layer,untill copy_to_user is invoked.
 *****************************************************************************/
//assume continuous buffer
static int get_block(ts_event_t * * block, int size)
{
	int cnt, rd;
	unsigned long flags;
	ts_event_t *p,*pTmp;
	if(NODATA())
		return 0;
		
	cnt = 0;
	
	save_flags(flags);
	cli();
	//critical section
	//get the actual size of data need read
	if(DSIZE()*sizeof(ts_event_t) >= size)
	{
		rd = size/sizeof(ts_event_t);
	}
	else
	{
		rd = DSIZE();
	}
	
	* block = p = get_data();
	cnt ++;
	
	while(p && (cnt < rd))
	{
		if(rptr == 0)
			break;
		p = get_data();
		cnt ++;
	}
	
	restore_flags(flags);
	////////////////////////////////add by lyk hhtech
	pTmp=*block;
	if(pTmp->pressure!=0){
		pTmp->x=10+(pTmp->x-430)*(230-10)/(3558-430);
		pTmp->y=10+(pTmp->y-290)*(310-10)/(2679-400);
	}
	//////////////////////////////////////////////////////
	return (cnt)*sizeof(ts_event_t);
	
}
/******************************************************************************
 * Function Name: get_data
 *
 * Input: 		void	:
 * Value Returned:	ts_event_t	: a 'touch' event data format
 *
 * Description: read a 'touch' event from data buffer
 *
 *****************************************************************************/
// no really read out the data, so no copy
static ts_event_t* get_data(void)
{
	ts_event_t *data;
	
	if(NODATA())
		return NULL;

	data = & (buffer[rptr]);

	TRACE("*** Read - x: 0x%04x, y: 0x%04x, rptr: 0x%04x, wptr: 0x%04x\n", data->x, data->y, (int)rptr, (int)wptr);

	
	NEXTI(rptr);
	return data;
}

/******************************************************************************
 * Function Name: digi_release
 *
 * Input: 		inode	:
 * 			filp	:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: release resource when close the inode
 *
 *****************************************************************************/
int digi_release(struct inode * inode, struct file * filp)
{
	//printk("digi_release: ----\n");
	digi_fasync(-1, filp, 0);
	//disable the ADC interrupt
	//disable_pen_interrupt();
	touch_pan_disable_inter();
	MOD_DEC_USE_COUNT;
	return 0;
}

/******************************************************************************
 * Function Name: digi_open
 *
 * Input: 		inode	:
 * 			filp	:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: allocate resource when open the inode
 *
 *****************************************************************************/
int digi_open(struct inode * inode, struct file * filp)
{
	//printk("digi_open: ----\n");
        MOD_INC_USE_COUNT;

	//spi_flush_fifo();
	//enable ADC interrupt
	touch_pan_disable_inter();
	//touch_pan_set_pos_inter();//set default polarity
	touch_pan_clear_inter();
	touch_pan_enable_inter();
	//touch_pan_set_neg_inter();	
	pen_timer_status = 0;
    return 0;
}

/******************************************************************************
 * Function Name: digi_fasync
 *
 * Input: 		fd	:
 * 			filp	:
 * 			mode	:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: provide fasync functionality for select system call
 *
 *****************************************************************************/
static int digi_fasync(int fd, struct file *filp, int mode)
{
	/* TODO TODO put this data into file private data */
	int minor = check_device( filp->f_dentry->d_inode);
	//lyk changed it
	//int minor = 0;//check_device( filp->f_dentry->d_inode);
	if ( minor == - 1)
	{
		printk("<1>spi_fasyn:bad device minor\n");
		return -ENODEV;
	}
	//hhtech lyk add it
	/*if(fd!=-1){
		return 0;
	}else{
		return 1;
	*/
	return( fasync_helper(fd, filp, mode, &ts_fasync) );
	//}
}

/******************************************************************************
 * Function Name: digi_ioctl
 *
 * Input: 		inode	:
 * 			filp	:
 * 			cmd	: command for ioctl
 * 			arg	: parameter for command
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: ioctl for this device driver
 *
 *****************************************************************************/
int digi_ioctl(struct inode * inode, 
		struct file *filp,
		unsigned int cmd , 
		unsigned long arg)
{
	int ret = -EIO;
	int minor;
	
	
	minor = check_device( inode );
	if ( minor == - 1)
	{
		printk("<1>spi_ioctl:bad minor\n");
		return -ENODEV;
	}
	
	printk("<1>digi_ioctl:minor=%08x cmd=%d\n",minor,cmd);
	
	return ret;
}

/******************************************************************************
 * Function Name: spi_poll
 *
 * Input: 		filp	:
 * 			wait	: 
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: support poll and select
 *
 *****************************************************************************/
unsigned int digi_poll(struct file * filp, struct poll_table_struct * wait)
{
	int minor;

	minor = check_device( filp->f_dentry->d_inode);
	if ( minor == - 1)
		return -ENODEV;

	poll_wait(filp, &digi_wait, wait);
	
	return ( NODATA() ) ? 0 : (POLLIN | POLLRDNORM);
}


/******************************************************************************
 * Function Name: digi_read
 *
 * Input: 		filp	: the file 
 * 			buf	: data buffer
 * 			count	: number of chars to be readed
 * 			l	: offset of file
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: read device driver
 *
 *****************************************************************************/
ssize_t digi_read(struct file * filp, char * buf, size_t count, loff_t * l)
{       //lyk hhtech add it
	//int nonBlocking = filp->f_flags & O_NONBLOCK;
	int nonBlocking = 1;//filp->f_flags & O_NONBLOCK;
	int minor;
	ts_event_t *ev;
	int cnt=0;

	minor = check_device( filp->f_dentry->d_inode );
	if ( minor == - 1)
		return -ENODEV;
	//printk("rptr=%d,wptr=%d\n",rptr,wptr);
	if( nonBlocking )
	{
		if( !NODATA() ) 
		{
			/* returns length to be copied otherwise errno -Exxx */
			//printk("before get_block\n");
			cnt = get_block(&ev, count) ;
			if(cnt > count){
				printk(KERN_ERR"Error read spi buffer\n");
				return -EINVAL;
			}
			//printk("After get_block,rptr=%d,wptr=%d\n",rptr,wptr);
			__copy_to_user(buf, (char*)ev, cnt );
			return cnt;
		}
		else
			return -EINVAL;
	}
    else
	{ 
		//for us,block =0; so change here.
		/* check , when woken, there is a complete event to read */
		while(1){
			if( !NODATA() )
			{

				cnt = get_block(&ev, count);
				if(cnt > count){
					printk(KERN_ERR"Error read spi buffer\n");
					return -EINVAL;
				}
				/* returns length to be copied otherwise errno -Exxx */


				__copy_to_user(buf, (char*)ev, cnt);
				return cnt;
			}
			else
			{
				interruptible_sleep_on(&digi_wait);
				if(signal_pending(current))
					return -ERESTARTSYS;
			}
		}
	}

}

/*******************************************************
*digitizer init function
*Parameters:None
*Return	
*	0	indicates SUCCESS
* 	-1	indicates FAILURE
*Description: in this function should initialize the SPI device 
as well as the external touch pannel hardware
********************************************************/
signed short __init digi_init(void)
{
	int tmp;
	int i;

	init_timer(&pen_timer);
	pen_timer.function = digi_sam_callback;
	
	tasklet_init(&digi_tasklet, digi_tasklet_action,(unsigned long)0);	

    g_digi_major = devfs_register_chrdev(0, MODULE_NAME, &g_digi_fops);
    // devfs_register_chrdev(g_digi_major, MODULE_NAME, &g_digi_fops);
 	if ( g_digi_major < 0 )
 	{
		TRACE("%s driver: Unable to register driver\n",MODULE_NAME);
		return -ENODEV;
	}
	//printk("g_digi_major=%d\n",g_digi_major);

	g_devfs_handle = devfs_register(NULL, MODULE_NAME, DEVFS_FL_DEFAULT,
				      g_digi_major, 0,
				      S_IFCHR | S_IRUSR | S_IWUSR,
				      &g_digi_fops, NULL);   	
				      
 /*
    g_proc_dir=create_proc_entry(MODULE_NAME, 0, NULL);
	g_proc_dir->read_proc=get_proc_list;
	g_proc_dir->data=NULL;				      
*/
	//printk("enter init touch pannel\n");
	///////////////////////////////////////////////////////////
	//lyk hhtech added it 
        /*
	*(volatile U32*)PTB_GIUS |= 0x1<<16; //GPC 16:GPIO function;
        *(volatile U32*)PTB_DDIR &= ~(0x1<<16); //16 input
        *(volatile U32*)PTB_PUEN |= 0x1<<16; //pull high
        //volatile U32*)PTC_ICR1 |= 0x50000000; //PC 15,PC14,negtive edge interrupt(01), pin 15
        //only need pb16
	*(volatile U32*)PTB_ICR2 |= 0x00000001; //PC16,negtive edge interrupt(01), pin 16~30
        *(volatile U32*)PTB_IMR  &= ~(0x1<<16);   //all 16 interrupts masked
        *(volatile U32*)PTB_ISR  |= 0x1<<16;    //clear all 16 interrupts(write 1 to clear)
	///////////////
	*(volatile U32*)PTB_IMR |= 0x1<<16; // enable pin 16
	*/
	///////////////////////////////////////////////
	
	touch_pan_set_inter();//set pen IRQ 
	//tmp = request_irq(TPNL_IRQ,
	tmp = request_irq(IRQ_EINT1,
			(void * )digi_isr,
			TPNL_INTR_MODE,
			MODULE_NAME,
			MODULE_NAME);
	if (tmp)
	{
		//printk("digi_init:cannot init major= %d irq=%d\n", g_digi_major, TPNL_IRQ);
        devfs_unregister_chrdev(g_digi_major, MODULE_NAME);
        devfs_unregister(g_devfs_handle);
		return -1;
	}
	//init SPI buffer
	if(-1 == init_buf()){
		printk("digi_init: cannot init spi buffer, exit\n");

		free_irq(IRQ_EINT1,MODULE_NAME);

        devfs_unregister_chrdev(g_digi_major, MODULE_NAME);
        devfs_unregister(g_devfs_handle);
		disable_irq(IRQ_EINT1);
		return -1;
	}

	init_waitqueue_head(&digi_wait);
	//printk("after register device\n");
	//first init touch pannel
	address_map();	
	spi_init();
		
	//printk("after set inter\n");
	touch_pan_disable_inter();//mask pen interrupt
	//printk("after disable inter\n");
	//init pen touch pannel in GPIO,disabled
	//init SPI module
	//printk("after spi_init\n");
	
	touch_pan_init();
	//printk("after pan init\n");
	touch_pan_enable();//enable touch pannel
	//touch_pan_set_neg_inter();
	//printk("before spi_tx_data\n");
	//d000 at first,I change to 
    spi_tx_data(0xd000);
    spi_tx_data(0x0000);
    spi_tx_data(0x9000);
    spi_tx_data(0x0000);
    spi_tx_data(0xD000);
    spi_tx_data(0x0000);
    spi_tx_data(0x9000);
    spi_tx_data(0x0000);
    spi_tx_data(0x0000);
    spi_tx_data(0x0000);
    spi_tx_data(0x0000);
    spi_tx_data(0x0000);
    spi_tx_data(0x0000);
    spi_tx_data(0x0000);
    
    touch_pan_disable();
	//printk("all ok\n");
    /* Delay for A/D Converter to be stable */
    for(i=0;i<50000;i++);
   
	return 0;
}

void __exit digi_cleanup(void)
{
	/*Do some cleanup work*/
	printk("clean up\n");
	disable_irq(IRQ_EINT1);
	free_irq(IRQ_EINT1, MODULE_NAME);

	tasklet_kill(&digi_tasklet);

    if(g_digi_major>0)
    {
        devfs_unregister_chrdev(g_digi_major, MODULE_NAME);
        devfs_unregister(g_devfs_handle);
    }

	
}

int init_module(void)
{
	return digi_init();
}

void cleanup_module(void)
{
	digi_cleanup();
}

//MODULE_PARM(irq_mask, "i");
//MODULE_PARM(irq_list, "1-4i");
//lyk hhtech deleted it
//MODULE_LICENSE("GPL");

//>>>>>Body
