/*
 * $Id: graphic.c,v 1.2 1999/11/12 13:03:26 till Exp $
 *
 * Graphics primitive drawing functions
 * derived from: graphic.c Pixy Graphic Library
 * derived from: lissa.c: Graphics demos
 *
 * Copyright (C) 2001  Chen Yang <support@hhcn.com>
 * Copyright (C) 1999  Till Krech <till@snafu.de>
 * Copyright (C) 1998  Kenneth Albanowski <kjahds@kjahds.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The code in this file is partially based on Kenneth Albanowskis work
 * for the lissa graphics demo.
 */

/******************Notice***********************************
 * The Default Font size for ASCII Font is 8x16
 * The Default Font size of Chinese Font is 16x16
 ***********************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fb.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "mathf.h"
#include "font_8x16.h"
#include "gui.h"

/*Define the correct Chinese Font File Path*/
#define CHINESE_FONT_FILE "hanzi"

#define ScreenHeight 320
#define ScreenWidth 240

typedef struct {
    unsigned short height;
    unsigned char data[0];
} Pattern;


const Pattern _BlackPattern =
{
  1,
    {~0}
};
const Pattern _WhitePattern =
{
  1,
  { 0 }
};
const Pattern _DarkGreyPattern =
{
  2,
  {
    (unsigned char)0x55555555,
    (unsigned char)0xCCCCCCCC
  }
};
const Pattern _LightGreyPattern =
{
  4,
  {
    (unsigned char)0x88888888,
    (unsigned char)0x00000000,
    (unsigned char)0x22222222,
    (unsigned char)0x00000000,
  }
};
const Pattern _MicroPengPattern =
{
  16,
  {
    0x3c,0x7e,0x56,0xaa,0x86,0x4e,0x7b,0xc3,0x83,0x83,0xc3,0xc7,0xbd,0x99,0x99,
    0xff
  }
};

static const Pattern *patterns[] = {
  /* This must correspond to the enum in pixy.h! */
  &_BlackPattern,
  &_WhitePattern,
  &_DarkGreyPattern,
  &_LightGreyPattern,
  &_MicroPengPattern,
};
//unsigned char getbit[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01}
static short masktab[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
static int screen_fd=-1;
FILE *C_Font;
unsigned char * screen_ptr=(unsigned char*)(0x0400),*E_Font=(unsigned char*)(0x8500);
static short screen_width=240,screen_height=320;
static short WinSX,WinSY,WinEX,WinEY;
static unsigned short X,Y,Color,P_Index,Mode=MODE_SRC;
//CopyMode pixy_copy_mode = Mode_SRC;

inline void setpixel(short x, short y, short color)
{
    if ((x<0) || (x>=screen_width) || (y<0) || (y>=screen_height))
		return;
    {
	    
   	unsigned char * loc = screen_ptr + ((y * screen_width*2 + x*2 ));
	if (color){
		*loc =0xff;*(loc+1)=0xff;
	}else{
		*loc = 0x0;*(loc+1)=0x0;
	}
		
	
/*	short mask = masktab[(x&7)];
    	unsigned char * loc = screen_ptr + ((y * screen_width + x )>>3);
		if (color)
			*loc |= mask;
		else
			*loc &= ~mask;
*/	

    }
}


/* Abrash's take on the simplest Bresenham line-drawing algorithm. 
 *
 * This isn't especially efficient, as we aren't combining the bit-mask
 * logic and addresses with the line drawing code, never mind higher
 * level optimizations like run-length slicing, etc.
 *
 */

static inline void draw_xish_line(short x, short y, short dx, short dy, short xdir)
{
	short dyX2=dy+dy;
	short dyX2mdxX2=dyX2-(dx+dx);
	short error=dyX2-dx;
	
	setpixel(x, y, Color);
	while (dx--) {
		if (error >= 0) {
			y++;
			error += dyX2mdxX2;
		} else {
			error += dyX2;
		}
		x += xdir;
		setpixel(x,y,Color);
	//	printf("Color1inline=%d\n",Color);
	}
}

static inline void draw_yish_line(short x, short y, short dx, short dy,short xdir)
{
	short dxX2=dx + dx;
	short dxX2mdyX2=dxX2-(dy+dy);
	short error=dxX2-dy;
	
	setpixel(x, y, Color);
	while (dy--) {
		if (error >= 0) {
			x+= xdir;
			error += dxX2mdyX2;
		} else {
			error += dxX2;
		}
		y++;
		setpixel(x,y, Color);
	}
}

void line(short x1, short y1, short x2, short y2)
{
	short dx,dy;
	
	if ( y1 > y2) {
		short t = y1;
		y1 = y2;
		y2 = t;
		t = x1;
		x1 = x2;
		x2 = t;
	}
	
	dx = x2-x1;
	dy = y2-y1;
	
	if (dx > 0) {
		if (dx > dy)
			draw_xish_line(x1, y1, dx, dy, 1);
		else
			draw_yish_line(x1, y1, dx, dy, 1);
	} else {
		dx = -dx;
		if (dx > dy)
			draw_xish_line(x1, y1, dx, dy, -1);
		else
			draw_yish_line(x1, y1, dx, dy, -1);
	}
	
	
}

/* One of Abrash's ellipse algorithms  */

void ellipse(short x, short y, short a, short b)
{
	short wx, wy;
	short thresh;
	short asq = a * a;
	short bsq = b * b;
	short xa, ya;
	
	setpixel(x, y+b, Color);
	setpixel(x, y-b, Color);
	
	wx = 0;
	wy = b;
	xa = 0;
	ya = asq * 2 * b;
	thresh = asq / 4 - asq * b;
	
	for (;;) {
		thresh += xa + bsq;
		
		if (thresh >= 0) {
			ya -= asq * 2;
			thresh -= ya;
			wy--;
		}
		
		xa += bsq * 2;
		wx++;
		
		if (xa >= ya)
		  break;
		
		
		setpixel(x+wx, y-wy, Color);
		setpixel(x-wx, y-wy, Color);
		setpixel(x+wx, y+wy, Color);
		setpixel(x-wx, y+wy, Color);
	}
	
	setpixel(x+a, y, Color);
	setpixel(x-a, y, Color);
	
	wx = a;
	wy = 0;
	xa = (bsq * a) << 1;
	
	ya = 0;
	thresh = (bsq >> 2) - bsq * a;
	
	for (;;) {
		thresh += ya + asq;
		
		if (thresh >= 0) {
			xa -= bsq + bsq;
			thresh = thresh - xa;
			wx--;
		}
		
		ya += asq + asq;
		wy++;
		
		if (ya > xa)
		  break;
		 
		setpixel(x+wx, y-wy, Color);
		setpixel(x-wx, y-wy, Color);
		setpixel(x+wx, y+wy, Color);
		setpixel(x-wx, y+wy, Color);
	}
}

inline void h_line(int x1,int x2,int y)
{
 static short ftab[]={0xff,0x7f,0x3f,0x1f,0x0f,0x7,0x3,0x1};
 static short btab[]={0x0,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe};
 short count;

  count=(x2-x1)>>3;

 if(count>1)
 {
  unsigned char *loc;
  
  loc=screen_ptr+((y*screen_width+x1)>>3);
  
  if(Color)
  { 
   *(loc)|=ftab[x1&7];      
   memset(loc+1,0xff,count-1);
   *(loc+count)|= ftab[7-(x2&7)];
  }
   else
   {
    *loc &=btab[x1&7];
    memset(loc+1,0,count-1);  
    *(loc+count)&=ftab[(x2&7)+1];
   }
 }
  else
    line(x1, y, x2, y);	  
}

/* Composites */

void rectangle(short x1, short y1, short x2, short y2)
{
	line(x1, y1, x2, y1);
	line(x2, y1, x2, y2);
	line(x2, y2, x1, y2);
	line(x1, y2, x1, y1);
}

void bar(short x1,short y1,short x2,short y2)
{	
	for(;y1<y2;y1++)
		h_line(x1,x2,y1);
}

inline void clip_screen(short in_x, short in_y, short in_w, short in_h, short *out_x, short *out_y, short *out_w, short *out_h) {
  short swp;
  if (in_w < 0) {
    in_x += in_w;
    in_w = -in_w;
  }
  if (in_h < 0) {
    in_y += in_h;
    in_h = -in_h;
  }
  if (in_x < 0) {
    in_w -= in_x;
    in_x = 0;
  }
  if (in_y < 0) {
    in_h -= in_y;
    in_y = 0;
  }
  if (in_w > screen_width) {
    in_w = screen_width;
  }
  if (in_h > screen_height) {
    in_h = screen_height;
  }
  if (in_w < 0) {
    in_w = 0;
  }
  if (in_h < 0) {
    in_h = 0;
  }
  *out_x = in_x;
  *out_y = in_y;
  *out_w = in_w;
  *out_h = in_h;
}
		
void patternfill( short dest_x,
		 short dest_y, 
		 short w,
		 short h,
		 unsigned char*dest,
		 short dest_units_per_line);

void  fillrectangle(short x1, short y1, short x2, short y2)
{
	short x, y, w, h,i,j;
	//clip_screen(x1, y1, x2-x1, y2-y1, &x, &y, &w, &h); 
	//patternfill(x, y, w, h, screen_ptr, screen_width>>3);
        for(i=x1;i<x2;i++)
	for(j=y1;j<y2;j++)
		{setpixel(i, j, 1);
  			}
}
void  fillrect(short x1, short y1, short x2, short y2)
{
        short x, y, w, h;
        clip_screen(x1, y1, x2-x1, y2-y1, &x, &y, &w, &h);
        patternfill(x, y, w, h, screen_ptr, screen_width>>3);
}


void bitblt(
		 short src_x,
		 short src_y,
		 short w,
		 short h,
		 short dest_x,
		 short dest_y, 
		 unsigned char *src,
		 short src_units_per_line, 
		 unsigned char *dest,
		 short dest_units_per_line
		 ) 
{
  /* todo: clip */
  register char dx;
  unsigned short x,y;
  unsigned char src_off, dest_off;
  short dest_n;
  unsigned char dest_beg_mask, dest_end_mask;
  
  /* goto line y */
  src += src_y * src_units_per_line;
  dest += dest_y * dest_units_per_line;
  
  /* goto UNIT-offset x */
  src += src_x >>3;
  dest += dest_x >>3;

  /* determine number of affected units per line */  
  dest_n = ((dest_x + w + 8 - 1) >>3) - (dest_x >>3);

  /* determine PIXEL-offsets */
  src_off = src_x & 7;
  dest_off = dest_x & 7;
  dx = dest_off - src_off;
  
  /* make masks to mask out untouchable destination */
  dest_beg_mask = ~((unsigned char)(-1) >> dest_off);
  dest_end_mask = (unsigned char)(-1) >> ((dest_off + w) & 7);
  if (dest_end_mask == (unsigned char)(-1)) {
    /* bit stupid */
    dest_end_mask = 0;
  }
  //printf("dx=%d\n", dx);
  //dump_unit("dest_beg_mask", dest_beg_mask);
  //dump_unit("dest_end_mask", dest_end_mask);
  for (y = 0; y < h; y++) {
    // process one line
    register unsigned char *sp;
    register unsigned char *dp;
    register unsigned char mask = 0;
    register unsigned char left, right;
    unsigned char s, d;
    sp = src;
    dp = dest;
    for (x = 0; x < dest_n; x++, sp++, dp++) {
      // process one unit
      if (dx < 0) {
	// first, don't care about masks
	left = *sp << (-dx);
	right = *(sp+1) >> (8 + dx);
      } else if (dx > 0) {
	// first, don't care about masks
	left = *(sp-1) << (8-dx);
	right = *(sp) >> (dx);
      } else {
	left = *sp;
	right = 0;
      }
      s = left | right;
      // combine with destination
      switch (Mode) {
      case MODE_SRC:
      default:
	d = s;
	break;
      case MODE_NOT_SRC:
	d = ~s;
	break;
      case MODE_SRC_OR_DST:
	d = *dp | s;
	break;
      case MODE_SRC_AND_DST:
	d = *dp & s;
	break;
      case MODE_SRC_XOR_DST:
	d = *dp ^ s;
	break;
      case MODE_NOT_SRC_OR_DST:
	d = *dp | ~s;
	break;
      case MODE_NOT_SRC_AND_DST:
	d = *dp & ~s;
	break;
      case MODE_NOT_SRC_XOR_DST:
	d = *dp ^ ~s;
	break;
      case MODE_SRC_OR_NOT_DST:
	d = ~*dp | s;
	break;
      case MODE_SRC_AND_NOT_DST:
	d = ~*dp & s;
	break;
      case MODE_SRC_XOR_NOT_DST:
	d = ~*dp ^ ~s;
	break;
      }
      mask = 0;
      if (x == 0) {
	mask |= dest_beg_mask;
      }
      if (x == dest_n-1) {
	mask |= dest_end_mask;
      }
      *dp = (*dp & mask) | (d & ~mask);
    }
    src += src_units_per_line;
    dest += dest_units_per_line;
  }
}

void patternfill( short dest_x,
		 short dest_y, 
		 short w,
		 short h,
		 unsigned char*dest,
		 short dest_units_per_line)  
{
  
  /* todo: clip */
  register char dx;
  unsigned short x,y;
  unsigned char src_off, dest_off;
  short dest_n;
  unsigned char dest_beg_mask, dest_end_mask;
  const Pattern *pattern = patterns[P_Index];
  
  /* goto line y */
  dest += dest_y * dest_units_per_line;
  
  /* goto UNIT-offset x */
  dest += dest_x / 8;

  /* determine number of affected units per line */  
  dest_n = (dest_x + w + 8 - 1) / 8 - dest_x / 8;

  /* determine PIXEL-offsets */
  dest_off = dest_x % 8;
  
  /* make masks to mask out untouchable destination */
  dest_beg_mask = ~((unsigned char)(-1) >> dest_off);
  dest_end_mask = (unsigned char)(-1) >> ((dest_off + w) % 8);
  if (dest_end_mask == (unsigned char)(-1)) {
    /* bit stupid */
    dest_end_mask = 0;
  }
  
  for (y = 0; y < h; y++) {
    // process one line
    register unsigned char *dp;
    register unsigned char mask = 0;
    unsigned char s=pattern->data[(dest_y+y)%pattern->height], d;
    dp = dest;
    for (x = 0; x < dest_n; x++, dp++) {
      // combine with destination
      switch (Mode) {
      case MODE_SRC:
      default:
	d = s;
	break;
      case MODE_NOT_SRC:
	d = ~s;
	break;
      case MODE_SRC_OR_DST:
	d = *dp | s;
	break;
      case MODE_SRC_AND_DST:
	d = *dp & s;
	break;
      case MODE_SRC_XOR_DST:
	d = *dp ^ s;
	break;
      case MODE_NOT_SRC_OR_DST:
	d = *dp | ~s;
	break;
      case MODE_NOT_SRC_AND_DST:
	d = *dp & ~s;
	break;
      case MODE_NOT_SRC_XOR_DST:
	d = *dp ^ ~s;
	break;
      case MODE_SRC_OR_NOT_DST:
	d = ~*dp | s;
	break;
      case MODE_SRC_AND_NOT_DST:
	d = ~*dp & s;
	break;
      case MODE_SRC_XOR_NOT_DST:
	d = ~*dp ^ ~s;
	break;
      }
      mask = 0;
      if (x == 0) {
	mask |= dest_beg_mask;
      }
      if (x == dest_n-1) {
	mask |= dest_end_mask;
      }
      *dp = (*dp & mask) | (d & ~mask);
    }
    dest += dest_units_per_line;
  }
}

short initgraph(void)
{
  struct fb_var_screeninfo screeninfo;
  
  screen_fd = open("/dev/fb0", O_RDWR);
  if (screen_fd == -1) 
  	{
      perror("Unable to open frame buffer device /dev/fb0");
      return 0;
	 }

  if (ioctl(screen_fd, FBIOGET_VSCREENINFO, &screeninfo)==-1) {
      perror("Unable to retrieve framebuffer information");
      return 0;
          }

  //screen_width = screeninfo.xres_virtual;
  screen_width = 240;//screeninfo.xres_virtual;
  //screen_height = screeninfo.yres_virtual;
  screen_height = 320;//screeninfo.yres_virtual;
  //lyk modified it
  //E_Font	= (unsigned char*)(screeninfo.english_font);
  //printf("E_Font Address %x %x\n",E_Font,screeninfo.english_font);
  //if(!E_Font) 
	  //E_Font=(unsigned char*)(0x8804);
	  //E_Font=(unsigned char*)(0x8812);
   E_Font=fontdata_8x16;//(unsigned char*)(0x0004a690);
  screen_ptr = mmap(NULL, ((screen_height * screen_width*2)/4096+1)*4096, PROT_READ|PROT_WRITE, /*0*/MAP_SHARED, screen_fd, 0);
  printf("sc_ptr=%x\n",screen_ptr);
  if (screen_ptr==MAP_FAILED) {
              perror("Unable to mmap frame buffer");
	      close(screen_fd);
	      return 0;
       }
  
  C_Font=fopen(CHINESE_FONT_FILE,"rb");
  
  if(!C_Font)
  	{
	 perror("Unable to open Chinese font file");
	 close(screen_fd);
	 return 0;
	}
  Color=1;	  
  return 1;
}

void closegraph()
{
  if(screen_fd!=-1) {
	close(screen_fd);
  }
  if(C_Font)
	  fclose(C_Font);
}

void XY_clearscreen(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2)
{unsigned short i=0, h_y=y2-y1+1,w_x=x2-x1+1;
// for(i=0;i<h_y;i++)
//  memset(screen_ptr+240*2*y1+2*x1+i*240*2,0,w_x*2);
 memset(screen_ptr+240*2*y1+2*x1,0,h_y*w_x*2);

}

void clearscreen()
{
	memset(screen_ptr,0,screen_width*screen_height*2);
}

void clearscreen_white()
{
        memset(screen_ptr,0xff,screen_width*screen_height*2);
}

void draw_xbm(short sx, short sy, short width, short height, char* pixel)
{
	short i, j, k, t,l=(width>>=3)*height,m,wid=screen_width>>3;
	short d,off;
	char *loc=screen_ptr+(off=((sy*screen_width+sx)>>3));
 
         for(k=0,i=0;i<height;i++,k+=wid)
          for(j=0;(j<width)&&(j<20);j++) 
	{
	      d=0;
	      for(m=0;m<8;m++)
	        if(pixel[k+j]&masktab[m]) d|=masktab[7-m];
	      t=k+j;
              if((t+off)>=3200) return;
	      loc[t]=d;
			       	
    	}
}

void draw_bmp(short sx, short sy, short rwidth, short height, char* pixel,unsigned short color,unsigned short groundcolor)
{
	short i, j, k;
        char flag=0;
	char *loc=screen_ptr+sx*2+sy*240*2;
	for(j=0;j<height;j++)
	for(k=0;k<rwidth;k++)
	 for(i=0;i<8;i++)
          {flag= *(pixel+j*rwidth+k) & masktab[i];
            if(flag)
            {// *(loc+j*240*2+k*16+i*2)=0x00;
             // *(loc+j*240*2+k*16+i*2+1)=0x00;
	      *(unsigned short*)(loc+j*240*2+k*16+i*2)=color;

             }
          else{// *(loc+j*240*2+k*16+i*2)=0xff;
               // *(loc+j*240*2+k*16+i*2+1)=0xff;
		*(unsigned short*)(loc+j*240*2+k*16+i*2)=groundcolor;                 
             }
 
	}

}
void my_draw_bmp(short x,short y,unsigned short width,unsigned short height,char *pixel)
{short i,j;
 long aver_size=((width+31)/32)*4;
 char masktab[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
 for(j=0;(y+j)<ScreenHeight&&(y+j)>=0&&j<height;j++)     //0---height;
  for(i=0;(x+i)<ScreenWidth&&(x+i)>=0&&i<width;i++)      //0---width ;
          if(*(pixel+j*aver_size+i/8) & masktab[i%8])//judge pixel "0"or "1";       
               { *(screen_ptr+(y+j)*ScreenWidth*2+2*(i+x))=0x00;
                 *(screen_ptr+(y+j)*ScreenWidth*2+2*(i+x)+1)=0x00;
                         }
             else{ *(screen_ptr+(y+j)*ScreenWidth*2+2*(i+x))=0xff;
                   *(screen_ptr+(y+j)*ScreenWidth*2+2*(i+x)+1)=0xff;
                }

}


inline void color_dot_bmp(short x,short y,char *buf)
 {if(y>0&&y<ScreenHeight)
    if(x>0&&x<ScreenWidth)
     {*(screen_ptr+y*240*2+2*x)=*buf;
      *(screen_ptr+y*240*2+2*x+1)=*(buf+1);
     }
  }
void color_555_draw_bmp(short x,short y,unsigned short width,unsigned short height,char *buf)
{   long i,j,offset;
    unsigned short tmp,red,green,blue;
   for(j=y;j<ScreenHeight&&(j-y)<height;j++)
        for(i=x;i<ScreenWidth&&(i-x)<width;i++)
           { offset=(j-y)*width*2+(i-x)*2;
             tmp=*(unsigned short*)(buf+offset);
             red=(tmp&0x7c00)<<1;
             green=(tmp&0x03e0)<<1;
             tmp&=0x001f;
             tmp|=red|green;
            *(unsigned short*)(screen_ptr+j*240*2+2*i)=tmp;
           } 
}

void color_565_draw_bmp(short x,short y,unsigned short width,unsigned short height,char *buf)
{   long i,j,offset;
    unsigned short tmp,red,green,blue;
   for(j=y;j<ScreenHeight&&(j-y)<height;j++)
        for(i=x;i<ScreenWidth&&(i-x)<width;i++)
           { offset=(j-y)*width*2+(i-x)*2;
             tmp=*(unsigned short*)(buf+offset);
             /*red=tmp&0xf800;
             green=(tmp&0x07e0);
             tmp=tmp&0x001f;
             tmp=red|green|tmp;
             */
             *(unsigned short*)(screen_ptr+j*ScreenWidth*2+2*i)=tmp;
           /*  *(screen_ptr+j*ScreenWidth*2+2*i)=*(buf+offset);
             *(screen_ptr+j*ScreenWidth*2+2*i+1)=*(buf+offset+1);*/
      
           }
}

/////////////
void testbmp()
{long i,j;
 short k=0x20;
for(j=0;j<160;j++)
{ for(i=0;i<240;i++)
     { *(screen_ptr+j*240*2+2*i)=0x00;
       *(screen_ptr+j*240*2+2*i+1)=0x40;
        }
//  k=k+2;
}
}
////////////
///////////////
/*  void ShowBMP_16_color(char *filename,short x,short y)
{ BMPHEAD bmp_inf;
  FILE *fp;
  char Tmp,c,*buf,*buf1,*buf2;
  int  width,height;
  int bmptype=0;
  long size,i,j,k;
  fp=fopen(filename,"rb");
  if(!fp) return;
fseek(fp,2L,0);
  fread(&bmp_inf.bfSize,1,52,fp);
  if(bmp_inf.biBitCount!=16)
        {
         fclose(fp);
         puts("Unsupported 16_color bitmap!\n");
         printf("bitcolor=%d\n",bmp_inf.biBitCount);
         return;
        }
s1: printf("%d* %d\n",bmp_inf.biWidth,bmp_inf.biHeight);
  size=bmp_inf.biWidth*bmp_inf.biHeight*2;
  buf=(char*)malloc(size);
  fseek(fp,70L,0);

  fread(buf,1,size,fp);
  fclose(fp);
  for(i=0;i<(bmp_inf.biHeight>>1);i++)
    for(k=i*bmp_inf.biWidth*2,j=0;j<bmp_inf.biWidth*2;j++)
       {
         Tmp=buf[k+j];
         buf[k+j]=buf[size-k-bmp_inf.biWidth*2+j];
         buf[size-k-bmp_inf.biWidth*2+j]=Tmp;
       }
     color_555_draw_bmp(x,y, bmp_inf.biWidth,bmp_inf.biHeight,buf);
 // else
//     color_565_draw_bmp(x,y,bmp_inf.biWidth,bmp_inf.biHeight,buf);
    free(buf);

}
*/

/*void ShowBMP_24_color(char *filename,short x,short y)
 {BMPHEAD bmp_inf;
  FILE *fp;
  char Tmp,c,*buf,*buf1,*buf2;
  int  width,height;
  int bmptype=0;
  long size,i,j,k,m;
  short red,green,blue;
  float redtmp,greentmp,bluetmp;
  fp=fopen(filename,"rb");
  if(!fp) return;
  fseek(fp,2L,0);
  fread(&bmp_inf.bfSize,1,52,fp);
  if(bmp_inf.biBitCount!=24)
        {
         fclose(fp);
         puts("Unsupported 24_color bitmap!\n");
         printf("bitcolor=%d\n",bmp_inf.biBitCount);
         return;
        }
  printf("%d* %d\n",bmp_inf.biWidth,bmp_inf.biHeight);
  size=bmp_inf.biHeight*((bmp_inf.biWidth*3+3)/4*4);
  buf=(char*)malloc(size);
  buf1=(char*)malloc(bmp_inf.biWidth*bmp_inf.biHeight*2);

  fseek(fp,54L,0);
                                                                                
  fread(buf,1,size,fp);
  fclose(fp);
  for(i=0;i<(bmp_inf.biHeight>>1);i++)
    for(k=i*((bmp_inf.biWidth*3+3)/4)*4,j=0;j<bmp_inf.biWidth*3;j++)
       {
         Tmp=buf[k+j];
         buf[k+j]=buf[size-k-bmp_inf.biWidth*3+j];
         buf[size-k-bmp_inf.biWidth*3+j]=Tmp;
       }
  for(i=0;i<(bmp_inf.biHeight);i++)
    for(k=i*((bmp_inf.biWidth*3+3)/4)*4,m=i*bmp_inf.biWidth*2,j=0;j<bmp_inf.biWidth;j++)
  { 
      red=(((buf[k+j*3])>>3))*2048;
      green=(buf[k+j*3+1]>>3)*64;
      blue=(buf[k+j*3+2]>>3); 
     *(unsigned short *)(buf1+m+j*2)=red|green|blue;
    }  
    color_565_draw_bmp(x,y,bmp_inf.biWidth,bmp_inf.biHeight,buf1);
    free(buf);free(buf1);
                                                                                
}
*/
void ShowBMP(char *filename,short x,short y)
{
  BMPHEAD bmp_inf;
  FILE *fp;
  char Tmp,*buf,*buf1,*buf2,c;
  int width,height;
  int red,green,blue;
  long aver_size,size,i,m,j,k;
  fp=fopen(filename,"rb");
  if(!fp) return;
fseek(fp,2L,0);
  fread(&bmp_inf.bfSize,1,52,fp);
//printf("biBiCount %x\n",bmp_inf.biBitCount);
  if(bmp_inf.biBitCount!=1&bmp_inf.biBitCount!=16&bmp_inf.biBitCount!=24)
        {
         fclose(fp);
         puts("Unsupported 2|16color bitmap!\n");
         printf("bitcolor=%d\n",*((short*)&bmp_inf.biBitCount));
         return;
        }
   else if(bmp_inf.biBitCount==1)                    //Show black and white                         
{                           
 printf("%d* %d\n",bmp_inf.biWidth,bmp_inf.biHeight);
 aver_size=((bmp_inf.biWidth+15)/16)*2; //line size
 printf("aversize=%d\n",aver_size);
 size=aver_size*bmp_inf.biHeight;
  buf=(char*)malloc(size);
fseek(fp,62L,0);
  fread(buf,1,size,fp);
  fclose(fp);
  for(i=0;i<(bmp_inf.biHeight>>1);i++)
    for(k=i*aver_size,j=0;j<aver_size;j++)
       {
         Tmp=buf[k+j];
         buf[k+j]=buf[size-k-aver_size+j];
         buf[size-k-aver_size+j]=Tmp;
       }

    for(i=0;i<size;i++)
     buf[i]^=0xff;
  printf("the width=%d\nthe height=%d\n",bmp_inf.biWidth,bmp_inf.biHeight);
  my_draw_bmp(x,y,bmp_inf.biWidth,bmp_inf.biHeight,buf);
   free(buf);
  }

   else if(bmp_inf.biBitCount==16)                              //show 16_bit BMP   SHOW_16_color_bmp
   {size=bmp_inf.biWidth*bmp_inf.biHeight*2;
  buf=(char*)malloc(size);
  fseek(fp,54L,0);                                                                fread(buf,1,size,fp);
  fclose(fp);
  for(i=0;i<(bmp_inf.biHeight>>1);i++)
    for(k=i*bmp_inf.biWidth*2,j=0;j<bmp_inf.biWidth*2;j++)
       {
         Tmp=buf[k+j];
         buf[k+j]=buf[size-k-bmp_inf.biWidth*2+j];
         buf[size-k-bmp_inf.biWidth*2+j]=Tmp;
       }
   color_555_draw_bmp(x,y, bmp_inf.biWidth,bmp_inf.biHeight,buf);
 // else
   //  color_565_draw_bmp(x,y,bmp_inf.biWidth,bmp_inf.biHeight,buf);
    free(buf);
}
   else if(bmp_inf.biBitCount==24)
	{// printf("%d* %d\n",bmp_inf.biWidth,bmp_inf.biHeight);
  size=bmp_inf.biHeight*((bmp_inf.biWidth*3+3)/4*4);
  buf=(char*)malloc(size);
  buf1=(char*)malloc(bmp_inf.biWidth*bmp_inf.biHeight*2);
                                                                                
  fseek(fp,54L,0);
                                                                                
  fread(buf,1,size,fp);
  fclose(fp);
  for(i=0;i<(bmp_inf.biHeight>>1);i++)
    for(k=i*((bmp_inf.biWidth*3+3)/4)*4,j=0;j<bmp_inf.biWidth*3;j++)
       {
         Tmp=buf[k+j];
         buf[k+j]=buf[size-k-bmp_inf.biWidth*3+j];
         buf[size-k-bmp_inf.biWidth*3+j]=Tmp;
       }
   for(i=0;i<(bmp_inf.biHeight);i++)
    for(k=i*((bmp_inf.biWidth*3+3)/4)*4,m=i*bmp_inf.biWidth*2,j=0;j<bmp_inf.biWidth;j++)
  {
     /* red=(((buf[k+j*3+2])>>3))*2048;
      green=(buf[k+j*3+1]>>3)*64;
      blue=(buf[k+j*3]>>3);*/
     red=((float)(buf[k+j*3+2]))/255*31+0.5;
     green=((float)(buf[k+j*3+1]))/255*63+0.5;
     blue=((float)(buf[k+j*3]))/255*31+0.5;
     *(unsigned short *)(buf1+m+j*2)=(red<<11)|(green<<5)|(blue);
     // *(unsigned short *)(buf1+m+j*2)=red|green|blue;
       }
    color_565_draw_bmp(x,y,bmp_inf.biWidth,bmp_inf.biHeight,buf1);
    free(buf);
    free(buf1);
                                                                                
}
}

void ShowBuf(char *Buf,short x,short y)
{
  BMPHEAD bmp_inf;
  char Tmp,*buf,*buf1,*buf2,c;
  int width,height;
  int red,green,blue;
  long aver_size,size,i,m,j,k;
  memcpy(&bmp_inf.bfSize,(Buf+2),52);
  
//printf("biBiCount %x\n",bmp_inf.biBitCount);
  if(bmp_inf.biBitCount!=1&bmp_inf.biBitCount!=16&bmp_inf.biBitCount!=24)
        {
         //fclose(fp);
         puts("Unsupported 2|16color bitmap!\n");
         printf("bitcolor=%d\n",*((short*)&bmp_inf.biBitCount));
         return;
        }
   else if(bmp_inf.biBitCount==1)                    //Show black and white                         
{                           
 printf("%d* %d\n",bmp_inf.biWidth,bmp_inf.biHeight);
 aver_size=((bmp_inf.biWidth+15)/16)*2; //line size
 printf("aversize=%d\n",aver_size);
 size=aver_size*bmp_inf.biHeight;
  buf=(char*)malloc(size);
  memcpy(buf,(Buf+62),size);

 //fseek(fp,62L,0);
 // fread(buf,1,size,fp);
 // fclose(fp);
  for(i=0;i<(bmp_inf.biHeight>>1);i++)
    for(k=i*aver_size,j=0;j<aver_size;j++)
       {
         Tmp=buf[k+j];
         buf[k+j]=buf[size-k-aver_size+j];
         buf[size-k-aver_size+j]=Tmp;
       }

    for(i=0;i<size;i++)
     buf[i]^=0xff;
  printf("the width=%d\nthe height=%d\n",bmp_inf.biWidth,bmp_inf.biHeight);
  my_draw_bmp(x,y,bmp_inf.biWidth,bmp_inf.biHeight,buf);
   free(buf);
  }

   else if(bmp_inf.biBitCount==16)                              //show 16_bit BMP   SHOW_16_color_bmp
   {size=bmp_inf.biWidth*bmp_inf.biHeight*2;
  buf=(char*)malloc(size);
  memcpy(buf,(Buf+54),size);
//  fseek(fp,54L,0);                                                                fread(buf,1,size,fp);
//  fclose(fp);
  for(i=0;i<(bmp_inf.biHeight>>1);i++)
    for(k=i*bmp_inf.biWidth*2,j=0;j<bmp_inf.biWidth*2;j++)
       {
         Tmp=buf[k+j];
         buf[k+j]=buf[size-k-bmp_inf.biWidth*2+j];
         buf[size-k-bmp_inf.biWidth*2+j]=Tmp;
       }
   color_555_draw_bmp(x,y, bmp_inf.biWidth,bmp_inf.biHeight,buf);
 // else
   //  color_565_draw_bmp(x,y,bmp_inf.biWidth,bmp_inf.biHeight,buf);
    free(buf);
}
   else if(bmp_inf.biBitCount==24)
	{ printf("%d* %d\n",bmp_inf.biWidth,bmp_inf.biHeight);
  size=bmp_inf.biHeight*((bmp_inf.biWidth*3+3)/4*4);
  buf=(char*)malloc(size);
  buf1=(char*)malloc(bmp_inf.biWidth*bmp_inf.biHeight*2);
  memcpy(buf,(Buf+54),size);
                                                                               
 // fseek(fp,54L,0);
                                                                                
 // fread(buf,1,size,fp);
//  fclose(fp);
  for(i=0;i<(bmp_inf.biHeight>>1);i++)
    for(k=i*((bmp_inf.biWidth*3+3)/4)*4,j=0;j<bmp_inf.biWidth*3;j++)
       {
         Tmp=buf[k+j];
         buf[k+j]=buf[size-k-bmp_inf.biWidth*3+j];
         buf[size-k-bmp_inf.biWidth*3+j]=Tmp;
       }
   for(i=0;i<(bmp_inf.biHeight);i++)
    for(k=i*((bmp_inf.biWidth*3+3)/4)*4,m=i*bmp_inf.biWidth*2,j=0;j<bmp_inf.biWidth;j++)
  {
    /*  red=(((buf[k+j*3+2])>>3))*2048;
      green=(buf[k+j*3+1]>>3)*64;
      blue=(buf[k+j*3]>>3);*/
     red=((float)(buf[k+j*3+2]))/255*31+0.5;
     green=((float)(buf[k+j*3+1]))/255*63+0.5;
     blue=((float)(buf[k+j*3]))/255*31+0.5;
     *(unsigned short *)(buf1+m+j*2)=(red<<11)|(green<<5)|(blue);
     // *(unsigned short *)(buf1+m+j*2)=red|green|blue;
       }
    color_565_draw_bmp(x,y,bmp_inf.biWidth,bmp_inf.biHeight,buf1);
    free(buf);
    free(buf1);
                                                                                
}
}

void V_scroll_screen(short height) //Up/Down Scroll
{
   short dir=(height<0);	

   if(dir) height=-height;
	   
   if(height<screen_height)
 	{
        long nBytes=height*240*2;///////////
        long nCount=240*2*(screen_height-height);
	    if(dir)  //Down Scroll
	       memmove(screen_ptr+nBytes,screen_ptr,nCount);
	      else     //Up Scroll
	       memmove(screen_ptr,screen_ptr+nBytes,nCount);
	   } 
   else
     clearscreen();	   
}

void H_scroll_screen(short width) //Left Scroll
{
 short dir=(width<0);
 
 if(dir) width=-width;
 
 if(width<screen_width)
 {
   if(width&7)   //Check whether it is byte aligned
  {
  short nCount=screen_width*screen_height>>3;
  static unsigned char mskr[]={0,1,3,7,0xf,0x1f,0x3f,0x7f},
	  	       mskl[]={0,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe},
		       buf[3200];
  unsigned char /*buf,*/c,flag=0,d;
  short start=(width>>3),length=(width&7),i,j,wid=(screen_width>>3),off;
  
  memcpy(buf,screen_ptr,nCount);
  if(dir)
  {
  for(off=0,j=0;j<screen_height;j++,off+=wid)
  {
   for(flag=0,i=wid-1;i>=start;i--)
   {
	d=c=*(buf+off+i);
	c<<=length;
	if(flag) c|=(flag>>(8-length));
	flag=d&mskl[length];
	*(buf+off+i)=c;
   }
  if(start)
    for(i=wid-start;i<wid;i++)
    {
	*(buf+off+i)=0;
    }
  }
  }
  else
  for(off=0,j=0;j<screen_height;j++,off+=wid)
  {
   for(flag=0,i=start;i<wid;i++)
   {
	d=c=*(buf+off+i);
	c>>=length;
	if(flag) c|=(flag<<(8-length));
	flag=d&mskr[length];
	*(buf+off+i)=c;
   }
  if(start)
    for(i=0;i<start;i++)
    {
	*(buf+off+i)=0;
    }
  }
  memcpy(screen_ptr,buf,nCount); 
  }
 else
  {
    short i,j=0,wid=screen_width>>3;
    
    width>>=3;
    for(i=0;i<screen_height;i++,j+=wid)
    {
	    memmove(screen_ptr+j+width,screen_ptr+j,wid-width);
	    memset(screen_ptr+j,0,width);
    }
  }
 }
 else
   clearscreen();
}

void textout(short x,short y,unsigned char *buf,unsigned short color,unsigned short groundcolor)
{
  int i,j,count=strlen(buf);
  short k,l,m=screen_width>>3;
  char pixel[32];

#ifdef FONT
  if(!font) {
		puts("Please Init Chinese Enviroment First!");
	//	printf(X\n");
                return;
           }
#endif  
if(x<0) x=0;
else if(x>240) x=240;
if(y<0) y=0;
else if(y>320) y=320;
  for(i=0;i<count;) //1
  {
   if((buf[i]>=161)&&(buf[i+1]>=161))///2
     {
       j=((buf[i]-161)*94+(buf[i+1]-161))<<5;
       fseek(C_Font,j,SEEK_SET);
       fread(pixel,32,1,C_Font);
       if((x+16)>screen_width) {
                             y+=16;
                             x=0;
                           }
       if(y+16>screen_height) {V_scroll_screen(y+16-screen_height);////up
                                XY_clearscreen(0,screen_height-16,screen_width-1,screen_height-1);
                                y=screen_height-16;
                              }

       draw_bmp(x,y,2,16,pixel,color,groundcolor);
       x+=16;  	       
       i+=2;
     } 
   else
     {
        if(buf[i]=='\n') {if(y+16>screen_height) {V_scroll_screen(y+16-screen_height);////up
                                XY_clearscreen(0,screen_height-16,screen_width-1,screen_height-1);
                                y=screen_height-16;
                              }
                          else y+=16;
	      		  x=0;	     
      			}
        else
        {if((x+8)>screen_width) {
                             y+=16;
                             x=0;
                           }
       if(y+16>screen_height) {V_scroll_screen(y+16-screen_height);////up
                               XY_clearscreen(0,screen_height-16,screen_width-1,screen_height-1);
                               y=screen_height-16;
                           }

	      draw_bmp(x,y,1,16,E_Font+(buf[i]<<4),color,groundcolor);	
	      x+=8;
        }
       i++;
     }///end 2
   }//end 1
  
}

void setmode(CopyMode mode)
{
  Mode=mode;
}

CopyMode getmode(void)	
{
  return Mode;	
}

void setcolor(short color)
{
  Color=color;
}

UINT getcolor(void)
{
	return Color;
}

void setfillpattern(PatternIndex index)
{
  P_Index=index;	
}

PatternIndex getfillpattern(void)
{
  return P_Index;	
}

void moveto(short x,short y)
{
	X=x;Y=y;
}

void lineto(short x,short y)
{
	line(x,y,X,Y);
	X=x;Y=y;
}
