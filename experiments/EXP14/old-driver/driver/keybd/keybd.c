/* driver/char/keybd.c 
 *  this is a keybd char device driver. 
 * Any problem pls contact support@hhcn.com
 */
#include <linux/module.h>
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

#define KEYBD_MAJOR 138

#define NOKEY   0

//keybd_arr is an keyboard_map array.
unsigned char  keybd_arr[5][5] = {{0,0,0,0,0},{0,0x58,0xdf,0x5f,0xdb},{0,0x53,0x4f,0xcf,0xc7},{0,0x50,0x9d,0x5d,0x8e},{0,0xde,0x9f,0x20,0x8b}};


static char key=NOKEY;


/*Controller register for port c & d*/
#define key_rGPCCON (*(volatile unsigned long *)key_r_GPCCON)
#define key_rGPDCON (*(volatile unsigned long *)key_r_GPDCON)

/*Pull-up disable register for port c & d*/
#define key_rGPCUP  (*(volatile unsigned long *)key_r_GPCUP)     
#define key_rGPDUP  (*(volatile unsigned long *)key_r_GPDUP) 

/*Data register for port c & d*/
#define key_rGPCDAT (*(volatile unsigned long *)key_r_GPCDAT) 
#define key_rGPDDAT (*(volatile unsigned long *)key_r_GPDDAT)

unsigned long key_r_GPCCON,key_r_GPCUP,key_r_GPCDAT;
unsigned long key_r_GPDCON,key_r_GPDUP,key_r_GPDDAT;

devfs_handle_t devfs_keybd;

int	keybd_open(struct inode *, struct file *);
int	keybd_release(struct inode *, struct file *);
ssize_t keybd_read(struct file *, char * , size_t );


static struct file_operations keybd_fops = {
	open:		keybd_open,
	read:		keybd_read,
	release:	keybd_release,
};



/* 
 * Open/close code for raw IO.
 */

int keybd_open(struct inode *inode, struct file *filp)
{

	printk("open ok\n");
	return 0;

}


ssize_t keybd_read(struct file *fp, char * buf,
                 size_t size)
{
    int row,line;
   
    row = 0;
    line = 0;

    key_rGPDCON &= 0xfff5fff5;	  //output
    key_rGPDCON |= 0x00050005;	  //output
    key_rGPDDAT &= 0xfcfc;	  // 0 --> d

    key_rGPCCON &= 0xffc03fff;        //input
    udelay(10000);
    if ((key_rGPCDAT & 0x0080) == 0)           row = 1;   //bit7
    else if ((key_rGPCDAT & 0x0100) == 0)      row = 2;   //bit8
    else if ((key_rGPCDAT & 0x0200) == 0)      row = 3;   //bit9
    else if ((key_rGPCDAT & 0x0400) == 0)      row = 4;   //bit10

    key_rGPCCON &= 0xffd57fff;        //output
    key_rGPCCON |= 0x00154000;        //output
    key_rGPCDAT &= 0xf87f;            //c->0

    key_rGPDCON &= 0xfff0fff0;	  		 //input
    udelay(10000);
	 
    if ((key_rGPDDAT & 0x0001) == 0)           line = 1;   //bit0
    else if ((key_rGPDDAT & 0x0002) == 0)      line = 2;   //bit1
    else if ((key_rGPDDAT & 0x0100) == 0)      line = 3;   //bit8
    else if ((key_rGPDDAT & 0x0200) == 0)      line = 4;   //bit9


    key = keybd_arr[row][line];
    put_user(key,buf);
    key = NOKEY;

    return 1;
}

int keybd_release(struct inode *inode, struct file *filp)
{
	printk("release ok\n");
	return 0;
}


int __init keybd_init(void)
{
	printk("*********************keybd_init**************\n");
	//address map
        key_r_GPCCON = ioremap(0x56000020,4);
        key_r_GPCDAT = ioremap(0x56000024,4);
        key_r_GPCUP = ioremap(0x56000028,4);

        key_r_GPDCON = ioremap(0x56000030,4);
        key_r_GPDDAT = ioremap(0x56000034,4);
        key_r_GPDUP = ioremap(0x56000038,4);
    
        key_rGPCUP &= 0xf87f;      //Pull_up for port c    
        key_rGPDUP &= 0xfcfc;    

        devfs_keybd = devfs_register(NULL, "keybd", DEVFS_FL_DEFAULT,
	            	 KEYBD_MAJOR, 0,
	              	 S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
	               	 &keybd_fops, NULL);
	
       return 0;
}

static void __exit keybd_exit(void)
{
 	devfs_unregister(devfs_keybd);

}

module_init(keybd_init);
module_exit(keybd_exit);

