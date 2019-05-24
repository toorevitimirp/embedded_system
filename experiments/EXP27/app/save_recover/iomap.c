#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <termios.h>
//#include <linux/ppcflash.h>
#include "flash.h"
#include "iomap.h"

int map_flag=0;

void output_rq(struct ioport_ioctl_request rq)
{
	unsigned char *p=NULL;
	long i;
	p = rq.buff;
	printf("rq.count=%d\n",rq.count);
	for(i=0;i<=rq.count;i++)
	{
		printf("%.2lx  ",*p);
		p++;
		if((i%10==0)&&(i!=0))
			printf("i=%ld\n",i);
	}
	return ;
}

int unmap(int fd,struct ioport_ioctl_request *rq)
{
	if(map_flag==1)
	{
		if(ioctl(fd,IOPORT_UNMAP,rq))
			return 1;
		map_flag=0;
	}
	return 0;
}


int iomap(int fd,struct ioport_ioctl_request *rq)
{
	if(map_flag==1)
	{
		printf("recover:map_flag==1\n");
		if(ioctl(fd,IOPORT_UNMAP,rq)){
			printf("recover:ioctl(fd,IOPORT_UNMAP,rq)) error\n");
			return 1;
		}
		map_flag=0;
	}
	if(ioctl(fd,IOPORT_MAP,rq)){
		printf("recover:ioctl(fd,IOPORT_MAP,rq) error\n");
		return 1;
	}
	map_flag=1;
	return 0;
}

int ioread(int fd,struct ioport_ioctl_request *rq)
{
	if(map_flag==0)
	{
		printf("you need do a map operate first!\n");
		return -1;
	}
	if(rq->buff==NULL){
		printf("error: buff is null!\n");
		return -1;
	}
	ioctl(fd,IOPORT_READ,rq);
	return 0;
}

int iowrite(int fd,struct ioport_ioctl_request *rq,int flag)
{
	if(map_flag==0)
	{
		printf("you need do a map operate first!\n");
		return -1;
	}
	if(rq->buff==NULL){
		printf("error: buff is null!\n");
		return -1;
	}
    	if(flag==0){
	    printf("ioctl,flag=0,IOPORT_WRITE=%08x\n",IOPORT_WRITE);
	    if(ioctl(fd,IOPORT_WRITE,rq)<0)
	    {
		    printf("ioctl error");
			return -1;
	    } 
	}else{	//write without erase flash
	    if(ioctl(fd,IOPORT_WRITE_CON,rq)<0)
	    {
		    printf("ioctl error");
			return -1;
	    } 
	}
	return 0;
}
