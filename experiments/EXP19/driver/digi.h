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

#ifndef __DIGI_H_MX2_
#define __DIGI_H_MX2_
//lyk hhtech add it
typedef unsigned int U32;       /* unsigned 32 bit data */
/*#define PTB_BASE_ADDR           0xF021C100
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
*/
#define MX1_ADS_BOARD
//#define MX21_ADS_BOARD

#ifdef MX21_ADS_BOARD
//lyk hhtech modified it
//#define TPNL_IRQ 					8
#define TPNL_IRQ 					1	
#define TPNL_INTR_MODE				SA_INTERRUPT|SA_SHIRQ
#define TPNL_PEN_UPVALUE			-999

#define SPI_TE_INT_BIT		0x00000001	

#define SPI_XCH_BIT			0x00000200	
#define SPI_RR_INT_BIT		0x00000010			
#define SPI_XCH_MASK		0xfffffdff
#define SPI_EN_BIT			0x00000400
#endif

#ifdef MX1_ADS_BOARD
//lyk hhtech modified it

//#define TPNL_IRQ 					62

//#define TPNL_IRQ 					12
#define TPNL_INTR_MODE				SA_INTERRUPT|SA_SHIRQ
#define TPNL_PEN_UPVALUE			-999

/*
#define SPI_XCH_BIT			0x00000100	
#define SPI_RR_INT			0x00000008
#define SPI_XCH_MASK	0xfffffeff//1 means initiate exchange,0 means idle
#define SPI_EN_BIT		0x00000200
*/
#endif


//#ifdef IO_ADDRESS(r) 
//#undef IO_ADDRESS(r)
//#endif

/*
#define IO_ADDRESS(r)          ((r)+0xf0000000)
#ifdef MX1_ADS_BOARD
#define REG_SPI_RXDATA 		(*((volatile unsigned long*)(IO_ADDRESS(0x00213000))))
#define REG_SPI_TXDATA 		(*((volatile unsigned long*)(IO_ADDRESS(0x00213004))))
#define REG_SPI_CONTRL		(*((volatile unsigned long*)(IO_ADDRESS(0x00213008))))
#define	REG_SPI_INTER		(*((volatile unsigned long*)(IO_ADDRESS(0x0021300C))))
#define REG_SPI_SAMPLE		(*((volatile unsigned long*)(IO_ADDRESS(0x00213014))))
#define REG_SPI_SOFTRESET	(*((volatile unsigned long*)(IO_ADDRESS(0x0021301C))))
#define REG_PC_DDIR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C200))))
#define REG_PC_ICONFA1		(*((volatile unsigned long*)(IO_ADDRESS(0x0021C20C))))
#define REG_PC_ICONFA2		(*((volatile unsigned long*)(IO_ADDRESS(0x0021C210))))
#define REG_PC_DR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C21C))))
#define REG_PC_GIUS			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C220))))
#define REG_PC_GPR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C238))))
#define REG_PC_PUEN			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C240))))
#define REG_PC_OCR1			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C204))))

#define REG_PA_DDIR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C000))))
#define REG_PA_ICONFA1		(*((volatile unsigned long*)(IO_ADDRESS(0x0021C00C))))
#define REG_PA_ICONFA2		(*((volatile unsigned long*)(IO_ADDRESS(0x0021C010))))
#define REG_PA_GIUS			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C020))))
#define REG_PA_GPR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C038))))
#define REG_PA_PUEN			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C040))))
#define REG_PD_DDIR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C300))))
#define REG_PD_ICONFA2		(*((volatile unsigned long*)(IO_ADDRESS(0x0021C310))))
#define REG_PD_ICONFB2		(*((volatile unsigned long*)(IO_ADDRESS(0x0021C318))))
#define REG_PD_SSR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C324))))
#define REG_PD_GIUS			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C320))))
#define REG_PD_ICR2			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C32C))))
#define REG_PD_IMR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C330))))
#define REG_PD_ISR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C334))))
#define REG_PD_GPR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C338))))
#define REG_PD_PUEN			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C340))))
#define REG_PD_OCR2			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C308))))
//lyk add it hhtech
#define REG_PB_SSR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C124))))
#define REG_PB_ISR			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C134))))
#define REG_PB_ICR2			(*((volatile unsigned long*)(IO_ADDRESS(0x0021C12C))))
//lyk hhtech added it
//#define TOUCH_INT_MASK				0x7fffffff
#define TOUCH_INT_MASK				0xfffeffff
#define TOUCH_INT_NEG_POLARITY		0x00000001
#define SPI_TE_INT		0x00000001
#define SPI_RR_INT		0x00000008

#endif
*/


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

#endif

