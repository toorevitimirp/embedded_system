/* gui.c: Graphics demos
 * 
 *	Programmed By Chen: Yang (support@hhcn.com)
 *	
 * 	
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include<sys/types.h>
#include<sys/stat.h>
#include <stdlib.h>

#include "gui.h"
void delay()		/*延时函数*/
{int i,j;

 for(i=0;i<1000;i++)
  for(j=0;j<7000;j++) {}
 }
main(int argc,char*argv[])
{
   short i,j,w,h;
   struct stat st;
   int color0,filelength;	
   PatternIndex p1=BlackPattern;
   
   /*定义缓冲区，并初始化*/
   char buf[20]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
   char buf1[700000];
   short  *o,*c;
   int *p;
   long *q;
   
   FILE *fp;
  
  /*初始化液晶屏*/
  initgraph();
  clearscreen();
  
  /*画出方框*/
  fillrect(10,10,100,100);
  
  /*在指定的坐标出显出彩色文本*/
  textout(0,130,"hello!\n\nA Student",0xf800,0x0000);
  
  /*以只读方式打开文件*/
  if((fp=fopen("bqb4.bmp","r"))==NULL)
        {
         /*如文件缺失则报错，返回*/
         printf("Can't find the file");
         return 1;
       }
  
  /*取得文件的状态参数，存放于结构体内*/
  stat("bqb4.bmp",&st);
  
  /*从结构体中取出文件长度*/
  filelength=st.st_size;
  delay();  

  /*文本输出*/
  textout(0,30,"这只是一个小实验\n",0xf900,0x0000);
  
  /*将文件内容读入缓冲区，刚才取得的文件长度应用于此*/
  fread(buf1,1,filelength,fp); 
  
  /*关闭文件，释放资源*/
  fclose(fp);
  
  /*从缓冲区进行液晶屏输出*/
  ShowBuf(buf1,0,0);

/*打开文件*/
fp=fopen("bqb6.bmp","r");

/*取得文件参数*/
stat("bqb6.bmp",&st);

/*取得文件长度*/
filelength=st.st_size;

/*读入缓冲区*/
fread(buf1,1,filelength,fp);
fclose(fp);

/*显示缓冲区*/
ShowBuf(buf1,0,0);

/*以下同理，可以改用ShowBMP函数，使用很方便，但显示时没有ShowBuf流畅！*/
fp=fopen("bqb8.bmp","r");
stat("bqb8.bmp",&st);
fread(buf1,1,st.st_size,fp);
fclose(fp);
delay();
ShowBuf(buf1,0,0);
fp=fopen("bqb10.bmp","r");
stat("bqb10.bmp",&st);
fread(buf1,1,st.st_size,fp);
fclose(fp);
delay();
ShowBuf(buf1,0,0);
delay();
fp=fopen("bqb12.bmp","r");
stat("bqb12.bmp",&st);
fread(buf1,1,st.st_size,fp);
fclose(fp);
delay();
ShowBuf(buf1,0,0);
}
