#ifndef _IOMAP_H_
#define _IOMAP_H_
struct ioport_ioctl_request {
	unsigned long map_base;
	unsigned long map_len;
	unsigned long offset;
	unsigned char * buff;
	unsigned long count;
};

#define IOPORT_MAP 0xf021
#define IOPORT_UNMAP 0xf022
#define IOPORT_READ 0xf023
#define IOPORT_WRITE 0xf024
#define IOPORT_WRITE_CON 0xf025

#endif
