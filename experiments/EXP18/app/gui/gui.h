/* 
 * $Id: gui.h,v 1.0 2001/02/12 19:03:26 till Exp $
 *
 * basic gui header file
 *
 * Copyright (C) 2001  Chen Yang <chyang@hhcn.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

typedef unsigned int UINT;

/*typedef struct{
     unsigned char id[2];
     long filesize;
     short reserved[2];
     long headsize;
     long infosize;
     long width;
     long height;
     short bits;
     short planes;
     long biCompression;
     long sizeimage;
     long biXpp;
     long biYpp;
     long biclrused;
     long biclrimportant;
}BMPHEAD;
*/

typedef struct{
     short bfType; //__attribute__((packed));
     long  bfSize; //__attribute__((packed));
     short bfReserved1;
     short bfReserved2;
     long bfOffBits;
     long biSize;
     long biWidth;
     long biHeight;
     short biPlanes;
     short biBitCount;
     long biCompression;
     long biSizeImage;
     long biXpp;
     long biYpp;
     long biClrUsed;
     long biClrImportant;
}BMPHEAD;


typedef enum {
  MODE_SRC,
  MODE_NOT_SRC,
  MODE_SRC_OR_DST,
  MODE_SRC_AND_DST,
  MODE_SRC_XOR_DST,
  MODE_SRC_OR_NOT_DST,
  MODE_SRC_AND_NOT_DST,
  MODE_SRC_XOR_NOT_DST,
  MODE_NOT_SRC_OR_DST,
  MODE_NOT_SRC_AND_DST,
  MODE_NOT_SRC_XOR_DST,
  MODE_NOT_SRC_OR_NOT_DST,
  MODE_NOT_SRC_AND_NOT_DST,
  MODE_NOT_SRC_XOR_NOT_DST,
  InvalidMode
} CopyMode;

typedef enum {
  BlackPattern = 0,
  WhitePattern,
  DarkGreyPattern,
  LightGreyPattern,
  MicroPengPattern,
  InvalidPattern
} PatternIndex;

extern unsigned char *screen_ptr;
extern short initgraph(void);
extern void closegraph(void);

extern void clearscreen(void);
extern void clearscreen_white(void);

extern void setpixel(short x,short y,short color);
extern short getpixel(short x,short y);

extern void setmode(CopyMode mode);
extern CopyMode getmode(void); 

extern void setcolor(short color);
extern UINT getcolor(void);

extern void setfillpattern(PatternIndex index);
extern PatternIndex getfillpattern(void);

extern void bar(short x1,short y1,short x2,short y2);
extern void ellipse(short x1,short y1,short x2,short y2);
extern void line(short x1, short y1, short x2,short y2);
extern void lineto(short x1, short y1);
extern void moveto(short x,short y);
extern void rectangle(short x1,short y1,short x2,short y2);
extern void textout(short x,short y,unsigned char *s,unsigned short color,unsigned short groundcolor);
extern void ShowBMP_16_color(char *filename,short x,short y);
extern void V_scroll_screen(short height); //Vertical Scroll ^ 
extern void H_scroll_screen(short width);  //Horizonal Scroll:<-->

extern void testbmp();
extern void fillrectangle(short x1, short y1, short x2,short y2);


