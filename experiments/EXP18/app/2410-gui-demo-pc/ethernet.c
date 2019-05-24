#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
//#include "../gui/graphic.c"
#define PORT 2000
//#define Max 60000

int pow(int x,int y)
{int a,b=1;
 for(a=0;a<y;a++)
	{b*=x;}
 return b;
}
int aiptoi(char * pszip,unsigned int* piip)
{
	char psziphere[17],*psztmp1,*psztmp2,*pchar;
	int i;
	bzero(psziphere,17);
	strcpy(psziphere,pszip);
	strcat(psziphere,".");
	for(i=0,psztmp1=psziphere,pchar=(char *)piip;i<4;i++)
	{
		if((psztmp2=strstr(psztmp1,"."))==NULL)
			return 0;	
		psztmp2[0]=0;
		*(pchar+i)=atoi(psztmp1);
		psztmp1=psztmp2+1;
	}
	return 1;	
}


//int trans(int argc,char **argv)
int trans(char *argv,char *bmpbuf,char *filename)
{
	int sockfd,yy;
	unsigned int uiip;
	char a,b,c,szrecvbuf[128],buf1[10],pool[2],str0[3];
	char head[8];
	int *phead=head+4,nsize=128,allsize=0,i,xx;
	int  filelength,length,buf2[10],buf3[10];
	struct sockaddr_in servaddr;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);                   
//	their_addr.sin_addr.s_addr=hostip;
	bzero(&(servaddr.sin_zero),8);     
//	bzero(&servaddr,sizeof(struct sockaddr_in));
/*	bzero(&(servaddr.sin_zero),8);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8888);*/
//	if(!aiptoi(argv[1],&uiip)||argc<=1)
	if(!aiptoi(argv,&uiip))
	{
		printf("the ip is not correct or lose the ip input!\n");
		return 0;
	}

	servaddr.sin_addr.s_addr=uiip; 
	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(struct sockaddr)))
	{
		printf("Can't connect to the server!\n");
		return 0;
	}
/////////////////////////send filename///////////////////////
	write(sockfd,filename,10);
//////////////////////////////////
        read(sockfd,pool,1);
	printf("%s\n",pool);
	length=(int)(pool[0]-48);
	printf("length=%d\n",length);
	read(sockfd,buf1,length);
	printf("buf1=%s,%d\n",buf1,strlen(buf1));
	for(i=0;i<length;i++)
		{buf2[i]=(int)(buf1[i])-48;}
	for(i=0;i<length;i++)
		{buf3[i]=buf2[i]*pow(10,length-1-i);}
	filelength=0;
	for(i=0;i<length;i++)
		{filelength+=buf3[i];}
	printf("%d\n",filelength);
	while(allsize<filelength)
 		{bzero(szrecvbuf,128);
 	 	 read(sockfd,head,8);
	 	 nsize=read(sockfd,szrecvbuf,128);
		 for(i=0;i<nsize;i++)
			{bmpbuf[allsize+i]=szrecvbuf[i];}
    	 	 allsize+=nsize;
		 yy=100*allsize/filelength;
		 if(yy<100)
			{a=(char)(yy/10+48);
			 b=(char)((yy-(yy/10)*10)+48);
			 str0[0]=a;str0[1]=b;str0[2]='\0';
			// printf("str0=%s\n",str0);
			 textout(190,100,str0,0xeeee,0x3333);
			}
		 if(yy==100)
			{textout(190,100,"100",0xeeee,0x3333);}
		  xx=150*yy/100;
		  fillrectangle(40,40,40+xx,90);
	 	// if(nsize)
	 	//	{printf("now size:%d this time %d times %d\n",allsize,nsize,allsize/128);}
        	if(nsize<=0)
		 {break;}
        	}
	bmpbuf[filelength]='\0';
	return 0;
}
void recvfile(char *bmpbuf,char *filename)
	{char ip[]="192.168.2.100";
	 trans(ip,bmpbuf,filename);
	}
