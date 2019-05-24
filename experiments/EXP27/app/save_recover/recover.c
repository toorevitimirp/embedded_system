#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
//#include <linux/ppcflash.h>
#include "flash.h"
#include "iomap.h"
//#define FLASH_LENGTH   	512
#if 0
char filename[10][32]={"/etc/config/chat.ttyS1","/etc/config/dhcpd.leases"};
int size=10;
#endif
int write_from_flash(struct ioport_ioctl_request rq)
{
        unsigned char *p=rq.buff;
        unsigned long addr=0;
        FILE *fp;
        unsigned long i,j,k,len;
        unsigned short value_w,temp_w;
        char fname[32];
        bzero(fname,32);
        ////////////////////////////////////////////////////////////////////////
        //Judge if this is a valid file
        for(k=0;k<2;k++){ 
		if(*(unsigned short *)(p+addr)==0xFFFF){
			printf("error");
			break;
		}
        //////////////////////////////////////////////////////////////////////
        //get filename
		printf("addr=%d\n",addr);
		if(addr%4!=0){
			addr+=4-addr%4;
		}
		printf("addr=%d\n",addr);
		strncpy(fname,(unsigned char *)(p+addr),32);
			addr+=32;
        ///////////////////////////////////////////////////////////////////////
        //write file length
		len=*(unsigned long *)(p+addr);
		addr+=4;
        ////////////////////////////////////////////////////////////////////////
		printf("fname is %s,len=%d\n",fname,len);
		// fp=fopen(fname,"w");
		sprintf(fname,"/tmp/hehe%d",k);
		fp=fopen(fname,"w");
		//fp=fopen("/tmp/hehe","w");
 
		if(fp==NULL){
			perror("open file error");
			return 1;
		}
        //    printf("len is %x\n",len);
		for(i=0;i<len;i++){
			fwrite((unsigned char *)(p+addr+i),sizeof(unsigned char),1,fp);
		}
		fclose(fp);
		addr+=i;
	}
	return 0;
}

int main()
{
	int fd;
	struct ioport_ioctl_request rq;
	int len;
	memset(&rq,0,sizeof(struct ioport_ioctl_request));
	if((fd=open(DEVICE_FLASH,O_RDONLY | O_NONBLOCK))<0)
	{
		perror("can not open device");
		exit(1);
	}
	map_flag=0;
	printf("length is %x\n",FLASH_LENGTH);
	rq.map_base = FLASH_PHY_ADDR;
	rq.map_len = FLASH_LENGTH;
	if(iomap(fd,&rq))
	{
		printf("can't iomap\n");
		exit(1);
	}
	rq.count = rq.map_len;
	rq.offset = 0;
	rq.buff = malloc(sizeof(char)*rq.count);
	if(ioread(fd,&rq)<0){
		if(rq.buff!=NULL)
			free(rq.buff);
		printf("ioread error\n");
		exit(1);
	}
//	output_rq(rq);
//	printf("before\n");
	len=write_from_flash(rq);
	printf("len is %x\n",len);
	unmap(fd,&rq);
	
#if 0	
	rq.map_base = FLASH_PHY_ADDR+len;
	rq.map_len = FLASH_LENGTH;
	if(iomap(fd,&rq))
	{
		printf("can't iomap\n");
		exit(1);
	}
	rq.count = rq.map_len;
	rq.offset = 0;
	rq.buff = malloc(sizeof(char)*rq.count);
	if(ioread(fd,&rq)<0){
		if(rq.buff!=NULL)
			free(rq.buff);
		printf("ioread error\n");
		exit(1);
	}
	output_rq(rq);
//	printf("before\n");
	write_from_flash(rq);
//	printf("after\n");
	unmap(fd,&rq);
#endif
	close(fd);
	exit(0);
}
