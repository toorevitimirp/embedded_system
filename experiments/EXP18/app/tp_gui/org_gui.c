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
void delay()
{int i,j;

 for(i=0;i<1000;i++)
  for(j=0;j<20000;j++) {}
 }
main(int argc,char*argv[])
{
   short i,j,w,h;
   struct stat st;
   int color0,filelength;	
 PatternIndex p1=BlackPattern;
   char buf[20]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
   char buf1[700000];
   short  *o;int *p;long *q;char *c;
   FILE *fp;
  initgraph();
  if((fp=fopen("test3.bmp","r"))==NULL)
        {printf("Can't find the file");
         return 1;
       }
  stat("test3.bmp",&st);
  filelength=st.st_size;
  clearscreen();
  fillrect(10,10,100,100);
textout(0,130,"中国计量学院计算机网络研究室\n 热烈欢迎参加研讨班的朋友们\n",0xf800,0x0000);
  delay();
  delay(); 
  textout(0,30,"中国计量学院\n在美丽的杭州\n",0xf900,0x0000);
 delay();
 delay();
  clearscreen();
  fread(buf1,1,filelength,fp);
  ShowBuf(buf1,0,0);
ShowBMP("13.bmp",0,0);
delay();
clearscreen();
ShowBMP("test1.bmp",0,0);
delay();
clearscreen();
ShowBMP("test3.bmp",0,0);
delay();
delay();
clearscreen();
ShowBMP("test4.bmp",0,0);
delay();
delay();
clearscreen();
ShowBMP("12.bmp",0,0);
}
