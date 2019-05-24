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
/******************************************************************************
 * 
 * File Name:   asp.h
 * 
 * Progammers:  Chen Ning
 *
 * Date of Creations:   10 DEC,2001
 *
 * Synopsis:
 *
 * Modification History:
 * 10 DEC, 2001, initialization version, frame work for frame buffer driver
 *
 *****************************************************************************/

#ifndef ASP_H
#define ASP_H

//#define MOD_NAME        "asp"
//#define DEV_IRQ_NAME	"asp"
//#define DEV_IRQ_ID	"asp"
#define MOD_NAME        "PenDevice"
#define DEV_IRQ_NAME	"PenDevice"
#define DEV_IRQ_ID	    "PenDevice"

// registers
//////////////////////////////////////////////lyk add it//hhtech
typedef unsigned int U32;       /* unsigned 32 bit data */
#define PTB_BASE_ADDR           0xF021C100
#define PTB_DDIR                PTB_BASE_ADDR
#define PTB_OCR1                (PTB_BASE_ADDR+0x04)
#define PTB_OCR2                (PTB_BASE_ADDR+0x08)
#define PTB_ICONFA1             (PTB_BASE_ADDR+0x0C)
#define PTB_ICONFA2             (PTB_BASE_ADDR+0x10)
#define PTB_ICONFB1             (PTB_BASE_ADDR+0x14)
#define PTB_ICONFB2             (PTB_BASE_ADDR+0x18)
#define PTB_DR                  (PTB_BASE_ADDR+0x1C)
#define PTB_GIUS                (PTB_BASE_ADDR+0x20)
#define PTB_SSR                 (PTB_BASE_ADDR+0x24)
#define PTB_ICR1                (PTB_BASE_ADDR+0x28)
#define PTB_ICR2                (PTB_BASE_ADDR+0x2C)
#define PTB_IMR                 (PTB_BASE_ADDR+0x30)
#define PTB_ISR                 (PTB_BASE_ADDR+0x34)
#define PTB_GPR                 (PTB_BASE_ADDR+0x38)
#define PTB_SWR                 (PTB_BASE_ADDR+0x3C)
#define PTB_PUEN                (PTB_BASE_ADDR+0x40)
/////////////////////////////////////////////////
#define ASP_BASE		0xf0215000
#define ASP_REGION_SIZE		0x0100

#define ASP_ACNTLCR		0xf0215010
#define ASP_PSMPLRG		0xf0215014
#define ASPCMPCNTL		0xf0215030
#define ASP_ICNTLR		0xf0215018
#define ASP_ISTATR		0xf021501c
#define ASP_PADFIFO		0xf0215000
#define ASP_CLKDIV		0xf021502c

// default value
// ASP_ACNTLCR
// CLKEN	:1
// BGE		:1
// SWRST	:1	 software reset ASP
// NM		:1
// SW		:0x020
// MOD		:10
// U_SEL	:0
// AZ_SEL	:0
// AZE		:1
#define ASP_ACNTLCR_DEF		0x0284e203
#define ASP_ACNTLCR_START	(ASP_ACNTLCR_DEF|0x02)
// DMCNT	:111
// BIT_SELECT	:00		-- 16b
// IDLECNT	:0x3f		-- 63 clocks
// DSCNT	:0xf		-- 15 clocks
#define ASP_PSMPLRG_DEF		0x000073ff

// compare control register
// INT		:?		-- triggered
// CC		:0		-- less
// INSEL	:01		-- x channel
// compare value:0x9000
#define ASPCMPCNTL_DEF		0x00019000
#define ASPCMPCNTL_INT		0x00040000

// PFFE		:1
// PDRE		:0
// POL		:0		-- active low
// EDGE		:1 ? edge
// PIRQE	:1		-- pen enable
#define ASP_ICNTLR_DEF		0x00000032

#define ASP_ISTATR_BGR		0x00000200
#define ASP_ISTATR_POV		0x00000080
#define ASP_ISTATR_PEN		0x00000040
#define ASP_ISTATR_PFF		0x00000002
#define ASP_ISTATR_PDR		0x00000001

// pen clock divide ratio is 5
#define ASP_CLKDIV_DEF		0x00000005

#define ASP_FIFO_SIZE	12

unsigned int READREG(unsigned int r)	
	{	
		volatile unsigned int * addr;
		addr = (unsigned int *)r;	
		return *(volatile unsigned int *)addr;	
	}
void WRITEREG(unsigned int r, unsigned int val) 
{
	volatile unsigned int * addr;
	addr = (unsigned int *)r;
	*(volatile unsigned int *)addr = val;
}

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

typedef struct asp_tag{
	int auto_zero_value;
	int pen_up_value;
}ASP_S;




void update_AZ_and_AV(ASP_S *);
void disable_auto_sample(void);
void enable_auto_sample(void);
void enable_pen(void);
void enable_pen_interrupt(void);
void set_pen_up_threshold(ASP_S *);
void clear_pen_interrupt_flag(void);
void clear_cmp_interrupt_flag(void);
void clear_data_interrupt_flag(void);
void clear_pen_FIFO(void);
void disable_pen_interrupt(void);
void disable_pen(void);
void enable_data_interrupt(void);
void disable_data_interrupt(void);
void disable_cmp_interrupt(void);

// TODO: what meant
#define  MAX_ID 	0x14

#ifdef QT_IPAQ
#define PENUP		0x0
#define PENDOWN		0xffff
#else
#define PENUP		0x0
#define PENDOWN		0xff
#endif

#define FIFOLEN		16
#endif //ASP_H
