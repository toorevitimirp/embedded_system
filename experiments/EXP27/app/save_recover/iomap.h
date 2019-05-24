#ifndef __IOMAP_H_
#define __IOMAP_H_
extern int map_flag;
int unmap(int fd,struct ioport_ioctl_request *rq);
int iomap(int fd,struct ioport_ioctl_request *rq);
int ioread(int fd,struct ioport_ioctl_request *rq);
int iowrite(int fd,struct ioport_ioctl_request *rq,int flag);

#define DEVICE_FLASH "/dev/flash_drv"
//#define FLASH_PHY_ADDR  0x403f0000
#define FLASH_PHY_ADDR  0x01530000
//#define FLASH_LENGTH	65000
#define FLASH_LENGTH	0x1000
#define SIZE	10
#define COUNT	2
//char filename[20][32]={"/etc/config/chat.ttyS1","/etc/config/dhcpd.leases"};
#endif
