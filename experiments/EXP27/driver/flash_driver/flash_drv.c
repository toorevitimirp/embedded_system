/* driver/char/ioport.c 
 * 
 * This file provide IO reading and writing from user space.
 * Pls create some device file in diretory /dev/flash_drv whose major is 218.
 * This driver support 0-255 devices. In user space user can read and write
 * IO through open and ioctl function provided by glibc.
 * 
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
///////////////////////////////
//lyk hhtech add it
#include "flash_drv.h"
#include "flash_file.h"
/////////////////////////////////////
#include <asm/io.h>
#include <linux/vmalloc.h>
#define dprintk(x...) 

#define IOPORT_MAJOR 200
/*define the flash_drv major node is 200*/

static int      g_flash_drv_major=0;
static devfs_handle_t g_flash_drv_devfs_handle;

typedef struct ioport_data_s {
	unsigned long base;
	unsigned long len;
	unsigned long io_lock;
	unsigned long maped;
} ioport_device_t;
unsigned long BaseOfFlash=0;

static ioport_device_t ioport_devices[256];


ssize_t	ioport_read(struct file *, char *, size_t, loff_t *);
ssize_t	ioport_write(struct file *, const char *, size_t, loff_t *);
int	ioport_open(struct inode *, struct file *);
int	ioport_release(struct inode *, struct file *);
int	ioport_ctl_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
int	ioport_ctl_read(int minor, struct ioport_ioctl_request* rq);
int	ioport_ctl_write(int minor, struct ioport_ioctl_request* rq);
int	ioport_ctl_write_con(int minor, struct ioport_ioctl_request* rq);

static struct file_operations ioport_fops = {
	read:		ioport_read,
	write:		ioport_write,
	open:		ioport_open,
	release:	ioport_release,
};

static struct file_operations ioport_ctl_fops = {
	ioctl:		ioport_ctl_ioctl,
	open:		ioport_open,
};

int __init ioport_init(void)
{
	int i;
	g_flash_drv_major =
                devfs_register_chrdev(0, "flash_drv", &ioport_ctl_fops);
        g_flash_drv_devfs_handle=
                devfs_register(NULL, "flash_drv", DEVFS_FL_DEFAULT,
                                      g_flash_drv_major, 0,
                                      S_IFCHR | S_IRUSR | S_IWUSR,
                                      &ioport_ctl_fops, NULL);

	
	//register_chrdev(IOPORT_MAJOR, "flash_drv", &ioport_ctl_fops);
	for (i = 0; i < 256; i++) {
		ioport_devices[i].base = 0;
		ioport_devices[i].io_lock = 0;
		ioport_devices[i].maped = 0;
	}


	return 0;
}

__initcall(ioport_init);


static void flash_drv_cleanup(void){
        devfs_unregister_chrdev(g_flash_drv_major, "flash_drv");
        devfs_unregister(g_flash_drv_devfs_handle);
        //unregister_chrdev(IOPORT_MAJOR,"flash_drv");
}

/* 
 * Open/close code for raw IO.
 */

int ioport_open(struct inode *inode, struct file *filp)
{
	int minor;

	minor = MINOR(inode->i_rdev);
/*	if (ioport_devices[minor].io_lock) {
		printk("Device is busy\n");
		return -1;
	}*/
	ioport_devices[minor].io_lock++;
	return 0;

}
//__ioremap
int ioport_release(struct inode *inode, struct file *filp)
{
	int minor;
	
	minor = MINOR(inode->i_rdev);
	if (ioport_devices[minor].base)
		iounmap((void*)(ioport_devices[minor].base));
	if (ioport_devices[minor].maped)
		ioport_devices[minor].maped--;
	if (ioport_devices[minor].io_lock)
		ioport_devices[minor].io_lock--;
	return 0;
}



/*
 * Deal with ioctls against the raw-device control interface, to bind
 * and unbind other raw devices.  
 */


int ioport_ctl_ioctl(struct inode *inode, 
		  struct file *flip,
		  unsigned int command, 
		  unsigned long arg)
{
	struct ioport_ioctl_request rq;
	int err = 0;
	int minor = MINOR(inode->i_rdev);
	
	switch (command) {
	case IOPORT_UNMAP:
		if (ioport_devices[minor].maped) {
			iounmap((void*)(ioport_devices[minor].base));
			iounmap((void*)BaseOfFlash);
			ioport_devices[minor].maped--;
			return 0;
		}
		printk("Device is not mapped\n");
		return -1;
		
	case IOPORT_MAP:
		err = copy_from_user(&rq, (void *) arg, sizeof(rq));
		if (err)
			break;
		if (ioport_devices[minor].maped) {
			printk("Device is mapped!!!!\n");
			err = -1;
			break;
		}
		BaseOfFlash = ioremap(0x01000000,4);
		(void *)(ioport_devices[minor].base) = ioremap(rq.map_base,rq.map_len);
		printk("Device is mapped on %x from base=%08x len=%08x\n",ioport_devices[minor].base,rq.map_base,rq.map_len);
		if (!ioport_devices[minor].base) {
			printk("Device map error\n");
			err = -1;
			break;
		}
		ioport_devices[minor].len = rq.map_len;
		ioport_devices[minor].maped++;
		break;
	case IOPORT_READ:
		if (!ioport_devices[minor].maped) {
			printk("device not mapped\n");
			err = -1;
			break;
		}
		err = copy_from_user(&rq, (void *) arg, sizeof(rq));
		if (err)
			break;
		return ioport_ctl_read(minor,&rq);	
	case IOPORT_WRITE:
		
		printk("Yes new version\n");
		if (!ioport_devices[minor].maped) {
			printk("device not mapped\n");
			err = -1;
			break;
		}
		err = copy_from_user(&rq, (void *) arg, sizeof(rq));
		if (err)
			break;
		return ioport_ctl_write(minor,&rq);
	case IOPORT_WRITE_CON:	//write continue (without erase flash)
		if (!ioport_devices[minor].maped) {
			printk("device not mapped\n");
			err = -1;
			break;
		}
		err = copy_from_user(&rq, (void *) arg, sizeof(rq));
		if (err)
			break;
		return ioport_ctl_write_con(minor,&rq);
	default:
		err = -EINVAL;
	}
	
	return err;
}

int ioport_read_b(int minor,struct ioport_ioctl_request *rq)
{
	unsigned char* buff = vmalloc(rq->count);
	//int len = rq->count;
	int len = rq->count / 2;
	int offset = rq->offset;
	//unsigned char* ptr = buff;
	unsigned short* ptr = buff;
	if (!buff)
		return -1;
	while(len--){
		//*ptr++ = *(volatile unsigned char*)(ioport_devices[minor].base + offset++);
		*ptr = *(volatile unsigned short*)(ioport_devices[minor].base + offset);
		//ptr += 2;
		ptr ++;
		offset += 2;
	}
	copy_to_user(rq->buff,buff,rq->count);
	vfree(buff);
	return 0;
	
}

int ioport_ctl_read(int minor, struct ioport_ioctl_request *rq)
{
	if (rq->count == 0) {
		printk("Read count is zero\n");
		return(-1);
	}
	return ioport_read_b(minor,rq);
}
/* bellow add by HHTECH for flash program*/
int mflash_check_operation_complete(unsigned int addr)
{
	int bit=0,loop;
	unsigned short data1;
	loop=1;
	*(volatile unsigned short *)(addr)=INTELJ3_RSR_CMD;
	data1=*(volatile unsigned short *)(addr);

	while(!(data1&0x80))	
 	{
		*(volatile unsigned short *)(addr)=INTELJ3_RSR_CMD;
	        data1=*(volatile unsigned short *)(addr);
	}
	if(data1&0x80)              //SR7 = '1'?
	{                     
		if(data1&0x40){       //SR6 = '1'?
			bit=1;
		}             //Erase Suspend
		else if(data1&0x04){        //SR2 = '1'?
			bit=1;
		}             //Program Suspend
      		else if(data1&0x20){        //SR5 = '1'   
			if(data1&0x10){           //SR4 = '1'
				bit=1;	      //Error Command Sequence
				printk("Error Command Sequence!");
				return 1;
			}
		}
		else if(data1&0x10){             //SR4 = '1'
			bit=1;               //Error Command Sequence
			printk("Error Command Sequence!");
			return 1;
		}
		else if(data1&0x08){               //SR3 = '1'
			bit=1;                          //Error VPEN < VPENLK
			printk("Error on VPEN < VPENLK!");	
			return 1;
		}
		else if(data1&0x02){              //SR1 = '1'
			bit=1;                            //Error Block Locked
			printk("Error! Block Locked!");
			return 1;
		}

		if(bit){
			*(volatile unsigned short *)(addr)=INTELJ3_CSR_CMD; //Clear Status Register
		}
	}
	return 0;
}

void flash_reset(){
	*(volatile unsigned short *)(BaseOfFlash)=0xffff;
}
void erase_sector(unsigned long offset)
{
	int addr;
	addr = offset;//ARM2410_FLASH_BASE + SECTOR_SIZE * sector_num;
	*(volatile unsigned short *)(addr)=INTELJ3_BERASE_CMD1;
	*(volatile unsigned short *)(addr)=INTELJ3_BERASE_CMD2;
	/* poll for ready */
	if (mflash_check_operation_complete(addr)) {
		printk("mflash_erase_sector error : status read\n");
		return;
	}
	flash_reset();
	return ;

#if 0
	*(volatile unsigned short *)(offset&0xfffff000|0xaaa)=0xaaaa;
	*(volatile unsigned short *)(offset&0xfffff000|0x554)=0x5555;
	*(volatile unsigned short *)(offset&0xfffff000|0xaaa)=0x8080;
	*(volatile unsigned short *)(offset&0xfffff000|0xaaa)=0xaaaa;
	*(volatile unsigned short *)(offset&0xfffff000|0x554)=0x5555;
	*(volatile unsigned short *)(offset)=0x3030;
#endif
}

void write_flash(unsigned long offset,unsigned short data)
{
                *(volatile unsigned short*)offset=INTELJ3_WP_CMD;
      		*(volatile unsigned short *)offset = data;

		/* poll for ready */
                if (mflash_check_operation_complete(offset)) {
                        printk("mflash_program_sector error : status read\n");
                        return;
                }
		*(volatile unsigned short *)(BaseOfFlash)=0xff;

#if 0
      	*((volatile unsigned short *)(offset&0xfffff000|0xaaa))=0xaaaa;
      	*((volatile unsigned short *)(offset&0xfffff000|0x554))=0x5555;
      	*((volatile unsigned short *)(offset&0xfffff000|0xaaa))=0xa0a0;
#endif
}
int ioport_write_b_con(int minor,struct ioport_ioctl_request *rq)
{
	unsigned char* buff = vmalloc(rq->count);
	int len = rq->count/2;
	//int len = rq->count;
	//int offset;
	unsigned long offset;
	int i;
	//unsigned char* ptr = buff;
	unsigned short * ptr = buff;
	unsigned short tmp;
	printk("i am here len is %d\n",len);
	if (!buff)
		return -1;
	copy_from_user(buff,rq->buff,rq->count);
	//printk("i am here2\n");
	offset = ioport_devices[minor].base;
	printk("i am here3 offset is %x\n",offset);
//	printk("before erase\n");
//	printk("After erase\n");
//	while(*(volatile unsigned short *)offset!=0xffff);
//	printk("begin\n");
	//while(*(volatile unsigned char *)offset!=0xff);
	while(len>0) {
		tmp=*ptr;
	printk("write begin\n");
		write_flash(offset,tmp);
	printk("write end\n");
		//*(volatile unsigned char*)(offset) = *ptr;
		//*(volatile unsigned short*)(offset) = tmp;
		printk("%x ",tmp);
		if(len%20== 0)
			printk("\n");
		//i= 10;
		//for(i=0;i<5;i++)	
		//	printk("");
		ptr++;
		//ptr += 0x2;
		//offset ++;
		offset +=0x2;
		//len-=0x2;
		len--;
	}
	vfree(buff);
	return 0;
}

int ioport_write_b(int minor,struct ioport_ioctl_request *rq)
{
	unsigned char* buff = vmalloc(rq->count);
	int len = rq->count/2;
	//int len = rq->count;
	//int offset;
	unsigned long offset;
	int i;
	//unsigned char* ptr = buff;
	unsigned short * ptr = buff;
	unsigned short tmp;
	if (!buff)
		return -1;
	copy_from_user(buff,rq->buff,rq->count);
	offset = ioport_devices[minor].base;
	printk("before erase,offset=%08x,len=%08x\n",offset,len);
	erase_sector(offset);
	printk("After erase,data=%04x\n",*(volatile unsigned short *)offset);
#if 1 
	while(*(volatile unsigned short *)offset!=0xffff){
		printk("data=%04x\n",*(volatile unsigned short *)offset);
	}
#endif
	printk("begin,len=%d=0x%x\n",len,len);
	//while(*(volatile unsigned char *)offset!=0xff);
	while(len>0) {
		tmp=*ptr;
		write_flash(offset,tmp);
		//*(volatile unsigned char*)(offset) = *ptr;
		//*(volatile unsigned short*)(offset) = tmp;
		printk("%.4lx ",tmp);
		if(len%10== 0)
			printk("\n");
		//i= 10;
		//for(i=0;i<5;i++)	
		//	printk("");
		ptr++;
		//ptr += 0x2;
		//offset ++;
		offset +=0x2;
		//len-=0x2;
		len--;
	}
	vfree(buff);
	return 0;
}
int ioport_ctl_write_con(int minor, struct ioport_ioctl_request *rq)
{
	
	printk("ioctl write continue\n");
	if (rq->count == 0) {
		printk("Write count is zero\n");
		return(-1);
	}
	return ioport_write_b_con(minor,rq);
}

int ioport_ctl_write(int minor, struct ioport_ioctl_request *rq)
{
	
	printk("Write count is not zero!!\n");
	if (rq->count == 0) {
		printk("Write count is zero\n");
		return(-1);
	}
	return ioport_write_b(minor,rq);
}

ssize_t ioport_write(struct file *filp, const char * buf, 
		 size_t size, loff_t *offp)
{
	
	int minor = MINOR(filp->f_dentry->d_inode->i_rdev);
	int len = size;
	int offset = 0;
	while(len--) {
		*(volatile unsigned char*)(ioport_devices[minor].base + offset) = *buf++;
	offset++;
	}
	return size;
}

ssize_t ioport_read(struct file *filp, char * buf, 
		 size_t size, loff_t *offp)
{
	int len = size;
	int offset = 0;
	int minor = MINOR(filp->f_dentry->d_inode->i_rdev);
	while(len--) {
		*buf++ = *(volatile unsigned char*)(ioport_devices[minor].base + offset++);
	}
	return size;
}
module_init(ioport_init);
module_exit(flash_drv_cleanup);

