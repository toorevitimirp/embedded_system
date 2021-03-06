#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include <ctype.h>
#define MYPORT 2000                                /*定义服务器的监听端口*/
#define Max 100                                    /*定义了服务器一次可以接受的字符数目*/
#define BACKLOG 10                                 /*BACKLOG指定在请求队列中允许的最大请求数，进入的连接请求将在队列中等待accept（）函数接受它们*/
main()
{int sock_fd,new_fd[5], numbytes,t;
 char buf[Max],filename[10];    /*sock_fd,new_fd是套接字描述*/
 int nsize,nnsize,i,j,filelength;
 struct sockaddr_in my_addr;                        /*服务器的地址结构体*/
 struct sockaddr_in their_addr;                     /*主机的地址结构体*/
 int sin_size;
 int allsize=0;
 FILE *fp;
 struct stat st;
 char szsendbuf[128],head[8],buf1[10],buf2[10],buf3[2],length;
 int *phead=head+4;
// struct stat st;
if((sock_fd=socket(AF_INET,SOCK_STREAM,0))==-1)    /*建立流式套接字描述符，错误�*/
	{perror("socket");
	 exit(1);
	}

/*服务器结构体的地址赋初值*/
 my_addr.sin_family=AF_INET;                       
 my_addr.sin_port=htons(MYPORT);                  /*服务器的端口号2000*/
 my_addr.sin_addr.s_addr=INADDR_ANY;
 bzero(&(my_addr.sin_zero),8);                    /*填充0，凑齐长度*/
 /*绑定*/
 if(bind(sock_fd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr))==-1)
 	{perror("bindB");                               /*绑定失败*/
	 exit(1);
	 }
 if(listen(sock_fd,BACKLOG)==-1)                 /*监听端口是否有请求*/
 	{perror("listen");                              /*监听失败*/
	 exit(1); 
	}
 t=0;
 while(1)
	{
  	sin_size=sizeof(struct sockaddr_in);
  	if ((new_fd[t]=accept(sock_fd,(struct sockaddr *)&their_addr,&sin_size))==-1)  /*某个客户申请的地址*/
   	{perror("accept");
    	continue;
    	}
  	printf("server:got connection from %s\n",inet_ntoa(their_addr.sin_addr));
  ////////////////////////read filename///////////////////////
	read(new_fd[t],filename,10);
	printf("%s\n",filename);
    //    filename[9]=0;
////////////////////////////////////////
	stat(filename,&st);
	if((fp=fopen(filename,"r"))==NULL)
    	{printf("Can't find the file");
     	 return 1;
    	}
	filelength=st.st_size;
	printf("file size :%d\n",filelength);
	//write(fd,(char *)&filelength,sizeof(int));
 	for(i=0;(filelength/10)!=0;i++)
	{buf1[i]=(char)((filelength-(filelength/10)*10)+48);
	 filelength=filelength/10;
	}
 	buf1[i]=(char)(filelength+48);
	buf1[i+1]='\0';
	for(j=0;j<=i;j++)
		{buf2[j]=buf1[i-j];}
	buf2[j]='\0';
	printf("%s,%d\n",buf2,j);
	length=(char)(j+48);
	buf3[0]=length;
	buf3[1]='\0';
	write(new_fd[t],buf3,1);
//	delay();
	write(new_fd[t],buf2,j);
//        delay();
  	nsize=128;
  	allsize=0;
	while(nsize == 128)
	{
		bzero(szsendbuf,128);
		nsize = *phead = fread(szsendbuf,1,128,fp);
		write(new_fd[t],head,8);
		nsize =write(new_fd[t],szsendbuf,nsize);
		allsize+=nsize;
		if(allsize)
			printf("now size :%d this time %d times :%d\n",allsize,nsize,allsize/128);
		if(nsize <= 0)
		{
			printf("Can't send data!\n");
			return 0;
		}
	}
	close(new_fd[t]);
	fclose(fp);
	t++;
      //  return 0;
	}
}

