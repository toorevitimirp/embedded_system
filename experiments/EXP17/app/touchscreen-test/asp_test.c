/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Copyright (C) 2002, 2003 Motorola Semiconductors HK Ltd
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <asm/fcntl.h>
#include <asm/mman.h>
#include <errno.h>
#include "asp.h"

char *fbp = 0;
#define LCDADDR fbp
#define SYS_ERR -1
#define SYS_OK 0
//typedef unsigned int U32;
typedef int S32;
typedef unsigned short U16;
typedef unsigned short * P_U16;
typedef unsigned char U8;
typedef U16	STATUS;
typedef short S16;

typedef struct _tag_RECT
{
	S32 left;
	S32 right;
	S32 top;
	S32 bottom;
}RECT,*P_RECT;

typedef struct
{
	U32 xFactor;
	U32 yFactor;
	U32 xOffset;
	U32 yOffset;
	U8  scale;
	
	RECT pan;
	
}PEN_CONFIG, *P_PEN_CONFIG;

PEN_CONFIG _gPenConfig;

STATUS  _PenCalibratePoint(P_RECT pScreen, P_RECT pPan)
{
	S32 tmp,w;
	RECT screen;
	RECT pan;
	_gPenConfig.scale=10;

	screen=*pScreen;
	pan=*pPan;
	
	_gPenConfig.pan=pan;
	
	tmp=pan.right-pan.left;
	if(tmp==0)
		return SYS_ERR;
	
	w=(screen.right-screen.left);
	_gPenConfig.xFactor=(w<<_gPenConfig.scale)/tmp;
	
	tmp=pan.bottom-pan.top;	
	if(tmp==0)
		return SYS_ERR;
	
	w=screen.bottom-screen.top;
	_gPenConfig.yFactor=(w<<_gPenConfig.scale)/tmp;
	
	if(_gPenConfig.xFactor==0)
		return SYS_ERR;
	
	if ((_gPenConfig.xFactor == 0) || (_gPenConfig.yFactor == 0))
    {
        return (SYS_ERR);
    }
    
	_gPenConfig.xOffset=pan.left- (screen.left<<_gPenConfig.scale)/_gPenConfig.xFactor;
	_gPenConfig.yOffset=pan.top - (screen.top<<_gPenConfig.scale) /_gPenConfig.yFactor;
		
	return SYS_OK;
}


S16 _PenConvertLCD( U16 value, U32 factor, U32 offset)
{
    S32     temp;

    temp = value;
    temp -= offset;
    
    if ( temp < 0)
    {
        /*  outside LCD area  ...  */
        temp = -temp;
        return (S16)(-((temp * factor) >> _gPenConfig.scale));
    }
    else
    {
        /*  within LCD area  */
        return ((S16)((temp * factor) >> _gPenConfig.scale));
    }
}

S16 _PenMapX(U16 x)
{
	return _PenConvertLCD(x,_gPenConfig.xFactor,_gPenConfig.xOffset);

}

S16 _PenMapY(U16 y)
{
	return _PenConvertLCD(y,_gPenConfig.yFactor,_gPenConfig.yOffset);
}

DrawPoint(int x,int y,U16 color)
{
	if(x<=0)
		return ;
	if(y<=0)
		return ;
	
	if(x>=240)
		return;
	if(y>=320)
		return;
		
//	*((P_U16)LCDADDR+y*320+x)=color;
//	if(x&1) //for silcon bug
//		*((P_U16)LCDADDR+y*240+x-1)=color;
//	else
//		*((P_U16)LCDADDR+y*240+x+1)=color;  

	*((P_U16)LCDADDR+y*240+x)=color;  
}
DrawLine(S16 x1, S16 y1, S16 x2, S16 y2, U16 color)
{
	int i=0;
	int d=0;
	if(abs(y2-y1)>abs(x2-x1))
	{
		if(y2>y1)
			d=1;
		else
			d=-1;
			
		for(i=y1;i!=y2;i+=d)
		{
			DrawPoint(x1+(i-y1)*(x2-x1)/(y2-y1),i,color);
		}
			
	}else
	{
		if(x2>x1)
			d=1;
		else
			d=-1;
		
		for(i=x1;i!=x2;i+=d)
		{
			DrawPoint(i,y1+(i-x1)*(y2-y1)/(x2-x1),color);
		}
	
	}
}

int main()
{
    int fbfd = 0;
    int tfd=0;
    ts_event_t ts;
    RECT rt,st;
    
    int tp_err=0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    int blank=0;
    long int screensize = 0;
    
    int x = 0, y = 0,z=0;
    long int location = 0;
    int r,g,b;
    unsigned short int t;
    int oldx,oldy;
    int i=0;
    oldx=-1;oldy=-1;
    
    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd < 0) {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        exit(3);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fbfd, 0);
    if ((int)fbp == -1) {
        printf("Error: failed to map framebuffer device to memory.\n");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    // Figure out where in memory to put the pixel
    
    memset(fbp, 0xFF,screensize);
    
    for ( y = 0; y < 320; y+=10 )
        for ( x = 0; x < 240; x++ )  
        	*((unsigned short int*)(fbp + y*240*2+x*2)) = 0xF800;

    for ( y = 0; y < 320; y++ )
        for ( x = 0; x < 240; x+=10 )  
        	*((unsigned short int*)(fbp + y*240*2+x*2)) = 0xF800;

    //tfd = open("/dev/touchpanel", O_RDONLY);
    tfd = open("/dev/digi", O_RDONLY);
    if (tfd < 0) {
        printf("Error: cannot open /dev/digi device.errno=%d\n",errno);
        exit(1);
    }
    
    
    rt.left=0x1bc;
    rt.top=0x242;
    rt.right=0xE82;
    rt.bottom=0xEA7;
	
    st.top=10;
    st.left=10; 
    st.right=240-10;
    st.bottom=320-10;
    _PenCalibratePoint(&st,&rt);
    
    printf("Now Draw Line One Screen\n");
    printf("Press Any Key to Exit\n");
    while(1)
    //for(i=0;i<100;i++)
    {
    	
    	if(read(tfd,&ts,sizeof(ts_event_t))==sizeof(ts_event_t))
    	{
		//lyk hhtech add it
    		printf("X: %d Y: %d Press:%d ,oldx=%d,oldy=%d\n",ts.x,ts.y,ts.pressure,oldx,oldy);
    		//DrawPoint(_PenMapX(ts.x),_PenMapY(ts.y),0x003f);
    		if(ts.pressure==0)
    		{	//oldx=-1;oldy=-1;
    			continue;
    		}
		//ts.x=10+(ts.x-445)*(230-10)/(3600-445);
        	//ts.y=10+(ts.y-400)*(310-10)/(3630-400);
		tp_err=0;
		tp_err=(oldx>ts.x)? oldx-ts.x:ts.x-oldx;
		//printf("err1=%d\n",tp_err);
                tp_err+=(oldy>ts.y)? oldy-ts.y:ts.y-oldy;
		//printf(" err=%d\n",tp_err);
    		if(oldx!=-1 && tp_err>0 && tp_err<30)
    		{
    			//DrawLine(_PenMapX(ts.x),_PenMapY(ts.y),
    			//	 _PenMapX(oldx),_PenMapY(oldy),0x003f);    			
    			DrawLine(ts.x,ts.y,
    				 oldx,oldy,0x003f);    			
    		}   	
    		oldx=ts.x;
    		oldy=ts.y;		
    	}
    	
    }
    
	
    munmap(fbp, screensize);
    close(fbfd);
    close(tfd);
    return 0;
}
