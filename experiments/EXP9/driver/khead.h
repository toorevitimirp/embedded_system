#ifndef _MYHEAD_H_
#define _MYHEAD_H_

#include <asm/dma.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/blkpg.h>
#include <linux/compatmac.h>
#include <linux/config.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <linux/i2c-id.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/pm.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/wrapper.h>
#include <linux/vmalloc.h>

#define TRUE 1
#define FALSE 0


/* Typedefs for integer types */
typedef unsigned char U8;	/* unsigned 8 bit data  */
typedef unsigned short U16;	/* unsigned 16 bit data */
typedef unsigned int U32;	/* unsigned 32 bit data */
typedef signed char S8;		/* signed 8 bit data  */
typedef short S16;		/* signed 16 bit data */
typedef long S32;		/* signed 32 bit data */

typedef U8 *P_U8;		/* unsigned 8 bit data  */
typedef U16 *P_U16;		/* unsigned 16 bit data */
typedef U32 *P_U32;		/* unsigned 32 bit data */
typedef S8 *P_S8;		/* signed 8 bit data  */
typedef S16 *P_S16;		/* signed 16 bit data */
typedef S32 *P_S32;		/* signed 32 bit data */

typedef U16 TEXT;		/* 16-bit text data */
typedef P_U16 P_TEXT;		/* 16-bit text data */

typedef U8 BOOL;		/* Boolean, TRUE/FALSE */

typedef void VOID;		/* void */
typedef void *P_VOID;		/* pointer to void */
#ifndef BOOLEAN
typedef signed char BOOLEAN;
typedef unsigned char UINT8;
typedef signed char SINT8;
typedef unsigned short UINT16;
typedef signed short SINT16;
typedef unsigned long UINT32;
typedef signed long SINT32;

typedef volatile BOOLEAN VBOOLEAN;
typedef volatile UINT8 VUINT8;
typedef volatile SINT8 VSINT8;
typedef volatile UINT16 VUINT16;
typedef volatile SINT16 VSINT16;
typedef volatile UINT32 VUINT32;
typedef volatile SINT32 VSINT32;
#endif				/* BOOLEAN */


/************************* Structure/Union Data types ************************/

typedef U16 STATUS;

/*  point structure  */
typedef struct {
	U16 x;			/*  x point  */
	U16 y;			/*  y point  */
} POINT, *P_POINT;

/*  area structure  */
typedef struct {
	POINT top;		/*  Top left corner  */
	POINT bottom;		/*  Bottom right corner  */
} AREA, *P_AREA;

#ifdef MYDEBUG
#define PDEBUG(fmt, args...) printk(fmt, ##args)
#define PLINE PDEBUG("<%s:%s:%d>\n",__FILE__,__FUNCTION__, __LINE__)
#define FUNC_START printk("<%s:%s entered>\n", __FILE__, __FUNCTION__)
#define FUNC_END printk("<%s:%s ended>\n", __FILE__, __FUNCTION__)
#else
#define PDEBUG(fmt, args...)
#define PLINE 
#define FUNC_START
#define FUNC_END
#endif

//#define DEBUG_I2CWR
#ifdef DEBUG_I2CWR
#define dprinti2cwr(str...) printk("<"__FUNCTION__"> "str)
#else
#define dprinti2cwr(str...)	// nothing
#endif

#endif
