#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
//#include <linux/flash_drv.h>
#include "flash.h"
#include "iomap.h"
#define STEP 1	

//char filename[SIZE][32]={"/etc/passwd","/etc/shadow","/etc/group","/etc/config/chat.ttyS1","/etc/config/dhcpd.leases"};
char filename[SIZE][32]={"/etc/passwd","/etc/inetd.conf"};
//char filename[SIZE][32]={"/tmp/passwd","/tmp/hehe"};
//char filename[10][32]={"/etc/config/dhcpd.leases","/etc/config/chat.ttyS1"};
int size=10;
long int getfilelength(char *fname){
	FILE *fp;
	long int i;
	int j;
	if((fp=fopen(fname,"r"))==NULL)
	{
		perror("open file fail");
		return 0;
	}
	fseek(fp,0L,2);
	i=ftell(fp);
	j=fclose(fp);
	return(i);
}
#if 0
int write_to_flash_con(char *filename,struct ioport_ioctl_request rq)
{
	int fd;
	FILE *fp=NULL;
	char *c;
	unsigned char *p;
	unsigned long i,j,len;
	unsigned char value_w,temp_w;
	i=0;
	if((fd=open(DEVICE_FLASH,O_RDONLY | O_NONBLOCK))<0)
	{
		perror("can not open device");
		exit(1);
	}
	map_flag=0;
	if(iomap(fd,&rq))
	{
		printf("can't iomap\n");
		exit(1);
	}
	rq.count = rq.map_len;
	rq.offset = 0;
	c = malloc(sizeof(char)*rq.count);
	p=(unsigned char *)&len;
	len=getfilelength(filename);
	printf("len is %x\n",len);
	if(len>rq.count){
		printf("the file's len is too long\n");
		return 1;
	}
	for(i=0;i<32;i+=STEP){
		*(c +i) = *(unsigned char *)(filename+i); 
	}
	printf("it is %x --%x\n",*(unsigned char *)p,*(unsigned char *)(p+3));
	*(c +i)=*(unsigned char *)p;
	i++;
	*(c +i)=*(unsigned char *)(p+1);
	i++;
	*(c +i)=*(unsigned char *)(p+2);
	i++;
	*(c +i)=*(unsigned char *)(p+3);
	i++;
	if((fp=fopen(filename,"r"))==NULL){
		perror("open file err");
		return 1;
	}
	for(;;){
		j=fread(&value_w,sizeof(unsigned char),1,fp);
		//value_w = exchangew(value_w);
		if(j!=1){
			if(ferror(fp) != 0)
				printf("over or wrong\n");
			else{
				printf("\nFile write successfully!\n");
				printf("i is %d\n",i);
			}
			break;
		}
		//*(c +i)=*(unsigned short *)(&value_w);
		*(c +i)=(unsigned char )value_w;
		i+=STEP;
	}
	printf("i is %d\n",i);
	fclose(fp);
	rq.buff = c;
	iowrite(fd,&rq);
	unmap(fd,&rq);
	close(fd);
	return i;
}
#endif
int write_to_flash(char fname[][32],struct ioport_ioctl_request rq,int flag)
{
	int fd;
	FILE *fp=NULL;
	char *c;
	unsigned char *p;
	unsigned long i,j,len;
	unsigned long count,init;
	unsigned char value_w,temp_w;
	int size=10;
	for(i=0;i<2;i++)
		printf("name is %s\n",fname[i]);
	i=0;
	if((fd=open(DEVICE_FLASH,O_RDONLY | O_NONBLOCK))<0)
	{
		perror("can not open device");
		exit(1);
	}
	map_flag=0;
	if(iomap(fd,&rq))
	{
		printf("can't iomap\n");
		exit(1);
	}
	rq.count = rq.map_len;
	rq.offset = 0;
	c = malloc(sizeof(char)*rq.count);

	p=(unsigned char *)&len;
#if 0 
	printf("len is %ld\n",len);
	if(len>rq.count){
		printf("the file's len is too long,rq.count=%d\n",rq.count);
		return 1;
	}
#endif
	init=0;
	size=2;
	i=0;
	for(count=0;count<size;count++){
		len=getfilelength(fname[count]);
		p=&len;
		printf("file lenth=%d=%x\n",len,len);
		for(j=0;j<32;j+=STEP){
			*(c +i) = *(unsigned char *)(fname[count]+j); 
			i++;
		}
		printf("it is %x --%x\n",*(unsigned char *)p,*(unsigned char *)(p+1));
		*(c +i)=*(unsigned char *)p;
		i++;
		*(c +i)=*(unsigned char *)(p+1);
		i++;
		*(c +i)=*(unsigned char *)(p+2);
		i++;
		*(c +i)=*(unsigned char *)(p+3);
		i++;
		if((fp=fopen(fname[count],"r"))==NULL){
			
		//	printf()
			perror(fname[count]);
			exit(255);
		}
		printf("filename=%s,len=%d\n",fname[count],len);
		for(;;){
			j=fread(&value_w,sizeof(unsigned char),1,fp);
			//value_w = exchangew(value_w);
			if(j!=1){
				if(ferror(fp) != 0)
					printf("over or wrong\n");
				else{
					printf("\nFile write successfully!\n");
					printf("i is %d\n",i);
					if(i%4!=0){
						i+=4-i%4;
					}
					printf("i=%d\n",i);
				}
				break;
			}
			//*(c +i)=*(unsigned short *)(&value_w);
			*(c +i)=(unsigned char )value_w;
			i+=STEP;
		}
		fclose(fp);
//		init=i;
	}
//	printf("i is %d\n",i);
	rq.buff = c;
	iowrite(fd,&rq,flag);
	unmap(fd,&rq);
	close(fd);
	return i;
}

int main()
{
	int fd;
	struct ioport_ioctl_request rq;
	unsigned char *p;
//	char filename[20][32];
	int base=0;
	int i=0;
	memset(&rq,0,sizeof(struct ioport_ioctl_request));
	rq.map_base = FLASH_PHY_ADDR;
	rq.map_len = FLASH_LENGTH;
	printf("FLASH_PHY_ADDR=%08x,FLASH_LENGTH=%08x\n",FLASH_PHY_ADDR,FLASH_LENGTH);
	for(i=0;i<2;i++)
		printf("name is %s\n",filename[i]);
	for(i=0;i<32;i++)
		printf("%x ",filename[0][i]);
	printf("\n");
	for(i=0;i<32;i++)
		printf("%x ",filename[1][i]);
	//	sprintf(filename,"%s","/etc/config/diald.conf");
	base=write_to_flash(filename,rq,0);
#if 0
	printf("base is %x\n",base);

	rq.map_base +=base+1;
	printf("mapbase is %x\n",rq.map_base);
	sprintf(filename,"%s","/etc/config/chat.ttyS1");
	base=write_to_flash(filename,rq,1); //writhout erase flash
	printf("WIRTE SUCC!\n");
#endif
	return 0;
}
