/* driver/char/skeleton.c 
 *  this is a skeleton char device driver. 
 * Any problem pls contact support@hhcn.com
 */
#include <module.h>
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

#define IOWRITE 0xf021
#define IOCLEAR 0xf022
#define SKELETON_MAJOR 220
#define  STRLEN   100

char skeleton_drvinfo[100];

devfs_handle_t devfs_skeleton;

int	skeleton_open(struct inode *, struct file *);
int	skeleton_release(struct inode *, struct file *);
int	skeleton_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
ssize_t skeleton_read(struct file *, char * , size_t , loff_t *);
ssize_t skeleton_write(struct file *, const char *, size_t , loff_t *);

static struct file_operations skeleton_fops = {
	ioctl:		skeleton_ioctl,
	open:		skeleton_open,
	read:		skeleton_read,
	write:		skeleton_write,
	release:	skeleton_release,
};

/* 
 * Open/close code for raw IO.
 */
int skeleton_open(struct inode *inode, struct file *filp)
{
	printk("open ok\n");
	return 0;
}


ssize_t skeleton_read(struct file *filp, char * buf,
                 size_t size, loff_t *offp)
{
    char  * _buf;
    _buf = skeleton_drvinfo;
    copy_to_user(buf,_buf,sizeof(skeleton_drvinfo));
//    printk("write: %s\n",_buf);
//    printk("sizeof(skeleton_drvinfo):%d\n",sizeof(skeleton_drvinfo));
    return 0;
}

ssize_t skeleton_write(struct file *filp, const char *buf,
                  size_t size, loff_t *offp)
{
    char * _buf;
    _buf = skeleton_drvinfo;
    copy_from_user(_buf,buf,sizeof(skeleton_drvinfo));
//    printk("write: %s\n",_buf);
//    printk("sizeof(skeleton_drvinfo):%d\n",sizeof(skeleton_drvinfo));
    return 0;
}

//__ioremap
int skeleton_release(struct inode *inode, struct file *filp)
{
	printk("release ok\n");
	return 0;
}

/*
 * Deal with ioctls against the raw-device control interface, to bind
 * and unbind other raw devices.  
 */
int skeleton_ioctl(struct inode *inode, 
		  struct file *flip,
		  unsigned int command, 
		  unsigned long arg)
{
	int err = 0;
	switch (command) {
		case IOWRITE:
			printk("write ok\n");
			return 0;
		case IOCLEAR:
			printk("clear ok\n");
			return 0;
		default:
			err = -EINVAL;
	}
	return err;
}


int __init skeleton_init(void)
{
  devfs_skeleton =
        devfs_register(NULL,"skeleton",DEVFS_FL_DEFAULT,
		       SKELETON_MAJOR, 0,
		       S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
		       &skeleton_fops,NULL);
        return 0;
}
void __exit skeleton_exit(void)
{
  devfs_unregister(devfs_skeleton);
//  return 0;
}

//__initcall(skeleton_init);
module_init(skeleton_init);
module_exit(skeleton_exit);
