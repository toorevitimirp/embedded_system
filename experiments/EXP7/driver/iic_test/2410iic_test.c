//====================================================================
// File Name : 2410iic_test.c
// Function  : S3C2410 IIC-bus Master Tx/Rx Testing Program
// Date      : July 16, 2004
// Version   : 0.0
//====================================================================

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/sound.h>
#include <linux/soundcard.h>

#include <linux/pm.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/semaphore.h>
#include <asm/dma.h>
#include <asm/arch/cpu_s3c2410.h>

#include "khead.h"


int init_module(void)
{
   int hehe;
   hehe=Test_Iic();
   printk("The test..... insmod ..... is  OK!\n");
   return 0;
}

void cleanup_module(void)
{
  printk("The test...... rmmod.....is OK!\n");
}

