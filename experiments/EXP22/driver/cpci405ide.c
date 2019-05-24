/*
 *  linux/drivers/ide/cpci405.c -- CPCI405 IDE Driver
 *
 *     Copyright (C) 2001 by Stefan Roese
 *
 *  This driver was written based on information obtained from the MacOS IDE
 *  driver binary by Mikael Forselius
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <linux/types.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>


/*
 *  Base of the IDE interface
 */
/*IDE register BASE address and offset value modified by HHTECH for outself*/

#define CPCI405_HD_BASE	0x10000000

/*
 *  Offsets from the above base (scaling 4)
 */

#if 0
#define CPCI405_HD_DATA	        0x00
#define CPCI405_HD_ERROR	0x01		/* see err-bits */
#define CPCI405_HD_NSECTOR	0x02		/* nr of sectors to read/write */
#define CPCI405_HD_SECTOR	0x03		/* starting sector */
#define CPCI405_HD_LCYL	        0x04		/* starting cylinder */
#define CPCI405_HD_HCYL	        0x05		/* high byte of starting cyl */
#define CPCI405_HD_SELECT	0x06		/* 101dhhhh , d=drive, hhhh=head */
#define CPCI405_HD_STATUS	0x07		/* see status-bits */
#define CPCI405_HD_CONTROL	0x0e		/* control/altstatus */
#endif

#define CPCI405_HD_DATA         0x40
#define CPCI405_HD_ERROR        0x44            /* see err-bits */
#define CPCI405_HD_NSECTOR      0x48            /* nr of sectors to read/write */
#define CPCI405_HD_SECTOR       0x4C            /* starting sector */
#define CPCI405_HD_LCYL         0x50            /* starting cylinder */
#define CPCI405_HD_HCYL         0x54            /* high byte of starting cyl */
#define CPCI405_HD_SELECT       0x58            /* 101dhhhh , d=drive, hhhh=head */
#define CPCI405_HD_STATUS       0x5C            /* see status-bits */
#define CPCI405_HD_CONTROL      0x38		//0x10c

#define CPCI405_IRQ_IDE         0   //SIU_IRQ0   //0x1f

static int cpci405ide_offsets[IDE_NR_PORTS] __initdata = {
  CPCI405_HD_DATA, CPCI405_HD_ERROR, CPCI405_HD_NSECTOR, CPCI405_HD_SECTOR,
  CPCI405_HD_LCYL, CPCI405_HD_HCYL, CPCI405_HD_SELECT, CPCI405_HD_STATUS,
  CPCI405_HD_CONTROL
};

static void *cpci405_ide_base_mapped;

#if 1
int cpci405ide_check_region(ide_ioreg_t start, unsigned int len)
{
  if (((unsigned long)start >= (unsigned long)cpci405_ide_base_mapped) && 
      (((unsigned long)start+len) <= (unsigned long)cpci405_ide_base_mapped+0x200))
    return 0;
  else
    return -1;
}
#endif

/*
 *  Probe for a CPCI405 IDE interface
 */

void __init cpci405ide_init(void)
{
	hw_regs_t hw;
	int index = -1;
	unsigned long vaddr;
#if 0
        ppc_ide_md.ide_check_region = cpci405ide_check_region;
#endif

#if 0
        printk("cpci405ide: physical address=%08lx\n",
               (unsigned long)CPCI405_HD_BASE);              /* test-only */
#endif
#if 0
        _reg_CS3_U=0x00003200;
        _reg_CS3_L=0x00008501;
	_FMCR &= 0xFFFFFFFD;
#endif
//HHTECH now use nGCS3
#if 0
	BWSCON &= 0xFFFFF0FF;
	BWSCON |= 0x500;
	BANKCON2 = 0x550;
#endif
	BWSCON &= 0xFFFF0FFF;
	BWSCON |= 0x5000;
	BANKCON3 = 0x550;

#if 0
	GPFCON &= 0xFFFFCFFF;
	GPFCON |= 0x2000;
	EXTINT0 &= 0xF0FFFFFF;
	EXTINT0 |= 0x04000000;
	EINTMASK &= 0xFFFFFFBF;
	INTMSK &= 0xFFFFFFEF;
	INTMOD &= 0xFFFFFFEF;
#endif
//HHTECH now use : EINT3
#if 0
	set_external_irq(IRQ_EINT6,EXT_FALLING_EDGE, GPIO_PULLUP_DIS);
	disable_irq(IRQ_EINT6);
	enable_irq(IRQ_EINT6);
#endif
	set_external_irq(IRQ_EINT3,EXT_FALLING_EDGE, GPIO_PULLUP_DIS);
	disable_irq(IRQ_EINT3);
	enable_irq(IRQ_EINT3);

	cpci405_ide_base_mapped = ioremap(CPCI405_HD_BASE, 0x200);
	
	      //ioremap((unsigned long) CPCI405_HD_BASE, 0x200) - _IO_BASE;
#if 0
        printk("cpci405ide: mapped address=%08lx\n",
               (unsigned long)cpci405_ide_base_mapped);      /* test-only */
#endif
        ide_setup_ports(&hw, (ide_ioreg_t)cpci405_ide_base_mapped,
                        cpci405ide_offsets, 0, 0, NULL, CPCI405_IRQ_IDE);
        index = ide_register_hw(&hw, NULL);
        printk("ioremap:%x\n",cpci405_ide_base_mapped);
	if (index != -1)
          printk("ide%x: CPCI405 IDE interface\n", index);

}
