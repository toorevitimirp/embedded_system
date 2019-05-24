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
void delay()		/*��ʱ����*/
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
   
   /*���建����������ʼ��*/
   char buf[20]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
   char buf1[700000];
   short  *o,*c;
   int *p;
   long *q;
   
   FILE *fp;
  
  /*��ʼ��Һ����*/
  initgraph();
  clearscreen();
  
  /*��������*/
  fillrect(10,10,100,100);
  
  /*��ָ����������Գ���ɫ�ı�*/
  textout(0,130,"hello!\n\nA Student",0xf800,0x0000);
  
  /*��ֻ����ʽ���ļ�*/
  if((fp=fopen("bqb4.bmp","r"))==NULL)
        {
         /*���ļ�ȱʧ�򱨴�������*/
         printf("Can't find the file");
         return 1;
       }
  
  /*ȡ���ļ���״̬����������ڽṹ����*/
  stat("bqb4.bmp",&st);
  
  /*�ӽṹ����ȡ���ļ�����*/
  filelength=st.st_size;
  delay();  

  /*�ı����*/
  textout(0,30,"��ֻ��һ��Сʵ��\n",0xf900,0x0000);
  
  /*���ļ����ݶ��뻺�������ղ�ȡ�õ��ļ�����Ӧ���ڴ�*/
  fread(buf1,1,filelength,fp); 
  
  /*�ر��ļ����ͷ���Դ*/
  fclose(fp);
  
  /*�ӻ���������Һ�������*/
  ShowBuf(buf1,0,0);

/*���ļ�*/
fp=fopen("bqb6.bmp","r");

/*ȡ���ļ�����*/
stat("bqb6.bmp",&st);

/*ȡ���ļ�����*/
filelength=st.st_size;

/*���뻺����*/
fread(buf1,1,filelength,fp);
fclose(fp);

/*��ʾ������*/
ShowBuf(buf1,0,0);

/*����ͬ�������Ը���ShowBMP������ʹ�úܷ��㣬����ʾʱû��ShowBuf������*/
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