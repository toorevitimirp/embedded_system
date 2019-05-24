
/*    IME.c  Chinese Input Method Support Routine
 *
 *    Programmed By Chen Yang (chyang@sina.com)
 *
 *    In order to improve efficient,The program first builds
 *    an Encoded Tree of PinYin so that in the processing of
 *    the input,it doesn't need to GO BACK!!!!! On 68XX328,I
 *    think it is better if avoid this.The structure is based
 *    on the TRIER-tree,Thanks to <<Data Structure>>. Also
 *    thanks to Visual C++'s TreeControl,It is that made this
 *    a reality!!!
 *
 */
#include <linux/delay.h>
#include <sys/types.h>
#include <linux/fb.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include "../gui/gui.h"

#define HXMAX	240
#define HYMAX	234
#define HXMIN	15
#define HYMIN	12

#define WIDTH	192
#define HEIGHT	160
#define STARTX	0
#define STARTY	0
//#define KEYBOARD "/font/keyboard.bmp"
//#define L_KEYBOARD "/font/l_keyboard.bmp"
#include "keyboard.xbm"
#include "l_keyboard.xbm"

//#define DEBUGTRACE

struct _keyboard{
	char ch;
	short startx,starty,endx,endy;
}kbd[]={
	{'1',1+STARTX,1+STARTY,9+STARTX,13+STARTY},
	{'2',11+STARTX,1+STARTY,19+STARTX,13+STARTY},
	{'3',21+STARTX,1+STARTY,30+STARTX,13+STARTY},
	{'4',32+STARTX,1+STARTY,40+STARTX,13+STARTY},
	{'5',42+STARTX,1+STARTY,50+STARTX,13+STARTY},
	{'6',52+STARTX,1+STARTY,60+STARTX,13+STARTY},
	{'7',62+STARTX,1+STARTY,70+STARTX,13+STARTY},
	{'8',72+STARTX,1+STARTY,80+STARTX,13+STARTY},
	{'9',82+STARTX,1+STARTY,90+STARTX,13+STARTY},
	{'0',92+STARTX,1+STARTY,100+STARTX,13+STARTY},
	{',',102+STARTX,1+STARTY,110+STARTX,13+STARTY},
	{'(',112+STARTX,1+STARTY,120+STARTX,13+STARTY},
	{')',122+STARTX,1+STARTY,129+STARTX,13+STARTY},
	{'.',131+STARTX,1+STARTY,139+STARTX,13+STARTY},
	{'-',141+STARTX,1+STARTY,149+STARTX,13+STARTY},
	{':',151+STARTX,1+STARTY,158+STARTX,13+STARTY},
	{'?',1+STARTX,15+STARTY,14+STARTX,28+STARTY},//16
	{'q',16+STARTX,15+STARTY,29+STARTX,28+STARTY},
	{'w',31+STARTX,15+STARTY,44+STARTX,28+STARTY},
	{'e',46+STARTX,15+STARTY,58+STARTX,28+STARTY},
	{'r',60+STARTX,15+STARTY,72+STARTX,28+STARTY},
	{'t',74+STARTX,15+STARTY,87+STARTX,28+STARTY},
	{'y',89+STARTX,15+STARTY,102+STARTX,28+STARTY},
	{'u',104+STARTX,15+STARTY,116+STARTX,28+STARTY},
	{'i',118+STARTX,15+STARTY,130+STARTX,28+STARTY},
	{'o',132+STARTX,15+STARTY,144+STARTX,28+STARTY},
	{'p',146+STARTX,15+STARTY,158+STARTX,28+STARTY},
	{'¢', 1+STARTX,30+STARTY, 14+STARTX, 43+STARTY}, //27
	{'Ö', 16+STARTX, 30+STARTY, 29+STARTX, 43+STARTY},
	{'a', 31+STARTX, 30+STARTY, 44+STARTX, 43+STARTY},
	{'s', 46+STARTX, 30+STARTY, 58+STARTX, 43+STARTY},
	{'d', 60+STARTX, 30+STARTY, 72+STARTX, 43+STARTY},
	{'f', 74+STARTX, 30+STARTY, 87+STARTX, 43+STARTY},
	{'g', 89+STARTX, 30+STARTY, 102+STARTX, 43+STARTY},
	{'h', 104+STARTX, 30+STARTY, 116+STARTX, 43+STARTY},
	{'j', 118+STARTX, 30+STARTY, 130+STARTX, 43+STARTY},
	{'k', 132+STARTX, 30+STARTY, 144+STARTX, 43+STARTY},
	{'l', 146+STARTX, 30+STARTY, 158+STARTX, 43+STARTY},
	{'û', 1+STARTX ,45+STARTY, 14+STARTX, 58+STARTY},//38
	{'Æ', 16+STARTX, 45+STARTY, 29+STARTX, 58+STARTY},
	{'´', 31+STARTX, 45+STARTY, 44+STARTX, 58+STARTY},
	{'z', 46+STARTX, 45+STARTY, 58+STARTX, 58+STARTY},
	{'x', 60+STARTX, 45+STARTY, 72+STARTX, 58+STARTY},
	{'c', 74+STARTX, 45+STARTY, 87+STARTX, 58+STARTY},
	{'v', 89+STARTX, 45+STARTY, 102+STARTX, 58+STARTY},
	{'b', 104+STARTX, 45+STARTY, 116+STARTX, 58+STARTY},
	{'n', 118+STARTX, 45+STARTY, 130+STARTX, 58+STARTY},
	{'m', 132+STARTX, 45+STARTY, 144+STARTX, 58+STARTY},
	{' ',146+STARTX, 45+STARTY, 158+STARTX, 58+STARTY}
};
typedef struct {
	unsigned short pressure;  
	unsigned short x;
	unsigned short y;
} TS_RET;

char * device = "/dev/touchscreen/0raw";
int screen_fd;

void init_handpad()
{  
	screen_fd = open(device, O_RDONLY);
        if (screen_fd == -1) {
              printf("Unable to open touch screen");
                exit(0);
        }
}
typedef struct {
        unsigned short pressure;
        unsigned short x;
        unsigned short y;
} TS_RET_HANDPAD;
int get_average_num(int x,int y,int z){
	if(x<y){
		if(y<z)
			return y;
		else if(x<z)
			return z;
		else return x;
	}else if(y>z)
		return y;
	else if(x<z)
		return z;
	else return x;
}
int get_handpad(unsigned short *x,unsigned short *y)
{	
	int i=0,x_sum=0,y_sum=0,ylast=0;
	//static int iCounter = 0;
	//static int xLast2=0,yLast2=0,xLast1=0,yLast1=0,xThis=0,yThis=0;
	for(i=0;i<1;)
	{
	    TS_RET_HANDPAD cBuffer;
	    read(screen_fd,&cBuffer,sizeof(TS_RET_HANDPAD));
	    if(cBuffer.pressure){
		    if(cBuffer.y == 1022 || cBuffer.x == 1022){
		    }else{
			x_sum += cBuffer.x;
			y_sum += cBuffer.y;
			ylast = cBuffer.y;
			i++;//printf("x=%d,y=%d\n",cBuffer.x,cBuffer.y);
		    }
	    }
	}
	/*if(iCounter ==0){
		xLast2= (y_sum/1-240)*146/(970-240);
	        yLast2 = (x_sum/1-190)*127/(712-190);
		iCounter ++;
		return 0;
	}else if(iCounter ==1) {
		xLast1= (y_sum/1-240)*146/(970-240);
	        yLast1 = (x_sum/1-190)*127/(712-190);
		iCounter ++;
		return 0;
	}else {
		xThis = (y_sum/1-240)*146/(970-240);
	        yThis = (x_sum/1-190)*127/(712-190);
		*x = get_average_num(xLast2,xLast1,xThis);
		*y = get_average_num(yLast2,yLast1,yThis);
		xLast2 = xLast1;xLast1 = xThis;
		yLast2 = yLast1;yLast2 = yThis;
		return 1;	
	}*/
	*x = (y_sum/1-240)*146/(970-240);
	*y = (x_sum/1-190)*127/(712-190);
	return 1;
}

#define GetChildItem(r)				((r)->child)
#define GetNextSiblingItem(r)		((r)->next)
#define GetParentItem(r)  			((r)->parent)
#define SetItemData(r,v)			do {(r)->start=v;} while(0)
#define GetItemData(r)				((r)->start)
#define GetItemText(r)				((r)->ch)

struct nodetype{
  char ch;    //For the character
  short start;//Start Position in the index file
  short end;  //End Position in the index file
  struct nodetype *child,*next,*parent;
}root[26];//The global allocation for the 'a'->'z' 's root

struct nodetype * InsertItem(char ch,struct nodetype *p,short val)
{
   struct nodetype *t,*q;
   t=(struct nodetype*)malloc(sizeof(struct nodetype));
   if(!t) {
	   printf("Not enough Memory!!\n");
	   return NULL;
   	  }

   t->ch=ch;
   t->parent=p;
   t->child=t->next=NULL;
   t->end=0;

   for(q=p->child;q&&q->next;q=q->next);

   if(q)
	{
	q->next=t;
	q->end=val;
	}
     else
	p->child=t;

   return t;
}

void BuildData();

short Initialize()
{
  char str[10],ch,buf[20];//,buf[2]={0,0};
  short i,start,len,index;
  struct nodetype *current,*t;
  FILE *fp=fopen("/font/py.idx","r");

  if(!fp){
	  printf("handpad:can not open /font/py.idx.\n");
	  return 0;
  }

  /*Initialize the Input Struct*/
  //root=(struct nodetype*)malloc(sizeof(struct nodetype)*26);
  for(i=0;i<26;i++)
    {
     root[i].start=root[i].end=0;
     root[i].parent=root[i].child=NULL;
     root[i].next=root+i+1;
    }
    root[25].next=NULL;

   /*Begin to Get the Index Information*/
   while(fgets(buf,20,fp))	   
   {
     //printf("%s",buf);
     for(index=0;buf[index]!='\t';index++)
     {
	     str[index]=buf[index];
	     //putchar(str[index]);
     }
        
     str[index]=0;
     start=atoi(buf+index+1);
     //nprintf("%s %d\n",str,start);
     len=strlen(str);
     index=str[0]-'a';
     current=root+index;

     if(!(current->start)&&index) SetItemData(current,start);

     if(len>1) {
			for(i=1;i<len;i++)
			{
			     ch=str[i];
			     if((t=GetChildItem(current))==NULL)
			     {
				current=InsertItem(ch,current,start);
				SetItemData(current,start);
				}else
				{
				if(GetItemText(t)==ch) current=t;
					 else
					for(;t;t=GetNextSiblingItem(t))
					 {
					 if(GetItemText(t)==ch) { current=t;break;}
					 }

					if(!t){
						current=InsertItem(ch,current,start);
						SetItemData(current,start);
						}
				}
			   }
			}
   }

   root['i'-'a'].start=root['i'-'a'].end=root['j'-'a'].start;
   root['u'-'a'].start=root['u'-'a'].end=root['w'-'a'].start;
   root['v'-'a'].start=root['v'-'a'].end=root['w'-'a'].start;

   for(i=0;i<25;i++)
    {
    root[i].end=root[i+1].start;
    }

   root[25].end=14668;

   BuildData();

   fclose(fp);

   return 1;
}

struct nodetype *GetNextItem(struct nodetype *current)
{
	struct nodetype *t,*p;
	t=GetNextSiblingItem(current);

	p=current;
	while(!t)
	{
		 p=GetParentItem(p);
		if(p)
		 t=GetNextSiblingItem(p);
		else
		 break;
	}
       return t;
}

void BuildNode(struct nodetype*node)//Depth First
{
   struct nodetype *p,*q;
   while(node)
     {
       p=node->child;
       while(p)
       {
       BuildNode(p);
       if(!(p->end))
		{
		 q=GetNextItem(p);
		 if(q)
		  p->end=q->start;
		 else
		  p->end=14668;
		}
       p=p->next;
       }
       node=node->next;
     }
}

void BuildData()
{
   BuildNode(root);
}

void DeleteNode(struct nodetype*node)//Depth First
{
   struct nodetype *p;
   while(node)
   {
	     p=node->next;
	     DeleteNode(node->child);
	     //free(node);
	     node=p;
   }
}

void Delete()
{
   short i;
   for(i=0;i<26;i++)
     DeleteNode(root[i].child);
   //free(root);
}

void FindCandidate(char*buf,short*start,short*end)
{
	static struct nodetype *current=NULL;
	static short olen;
	short i,len;
	struct nodetype *t,*p;

	len=strlen(buf);
	//printf("FindCandidate:Current Buf:%s len=%d,current=%d,olen=%d\n",buf,len,current,olen);
	///////////////len=5,olen=4;
	for(i=0;i<len;i++){
		if(buf[i]<'a' || buf[i]>'z'){
			buf[i]=0;
			return;
		}
	}
    
	if(len>0)
	{
		if(!current)
		{
			i=buf[0]-'a';
/*			if(i<0)
			{
				for(i=0;i<10;i++)
					buf[i]=0;
				return;
			}
*/
			current=root+i;
			for(i=1;i<len;i++)
				{
				t=GetChildItem(current);
				if(t==NULL){
					buf[i]=0;
					return;
				}
				if(GetItemText(t)==buf[i]) 
					current=t;
				 else
					{
					 for(;t;t=GetNextSiblingItem(t))
					 {
						 if(GetItemText(t)==buf[i]) {	
							 current=t;
							 break;
						 }
					 }

					 if(!t)
					 {
					 buf[i]=0;
					 //printf("FindCandidate:could not found this PinYin\n");
					 //m_tree.SelectItem(current);
					 //m_edit.SetWindowText(buf);

					 /******************************/
					 /***Should Refresh the Input***/
					 /******************************/
					 return;
					 }
					}
				}
		}else{
		if(len>olen) {
				t=GetChildItem(current);
				if(t==NULL){
					buf[len-1]=0;
					return;
				}
				if(GetItemText(t)==buf[olen]) 
					current=t;
				 else
					{
					 for(;t;t=GetNextSiblingItem(t))
					 {
						 if(GetItemText(t)==buf[olen]) {	current=t;break;}
					 }
					 if(!t)
					 {
					 buf[len-1]=0;
					 //m_tree.SelectItem(current);
					 //m_edit.SetWindowText
					 //m_edit.SetWindowText(buf);
					 //m_edit.SetSel(olen,olen);

					 /******************************/
					 /***Should Refresh the Input***/
					 /******************************/

					 return;
					 }
				 }
			}else if(olen>len)
				current=GetParentItem(current);
			else return;

		}

		olen=len;
		*start=GetItemData(current);
		*end=current->end;
	}else
		current=NULL;
}


int main(void)
{
	unsigned short  i,x,y,index=0xffff,it;
	char buf[10],can[20];
	short Upper=1,count=0,start,end;
	FILE *fp=fopen("/font/gb.txt","rb");
	if(fp==NULL){
		printf("handpad:can not open /font/gb.txt!\n");
		return 0;
	}
	//printf("handpad:begin\n");
	init_handpad();
	//printf("handpad: begin initgraph()\n");
 	initgraph();
	//printf("handpad:begin clearscreen()\n");
	clearscreen();
	draw_xbm(0,0,xbm_keyboard_width,xbm_keyboard_height,xbm_keyboard_bits);
	setmode(MODE_SRC_XOR_DST);
	rectangle(0,60,159,127);
	if(!Initialize()) return;
	while(1)
	{
	    
	    if(get_handpad(&x,&y))
	    {
	    if(index!=0xffff)
    		{
		fillrect(kbd[index].startx,kbd[index].starty,
			kbd[index].endx+1,kbd[index].endy+1);
	    	}
	    //printf("x=%d,y=%d\n",x,y);
	    if(y<58) 
    		{
		if((y>kbd[0].starty)&&(y<kbd[0].endy))   {
		 for(i=0;i<16;i++)
			if((x>kbd[i].startx)&&(x<kbd[i].endx))
				break;
		if(i==16) index=0xffff;else index=i;		
		} else 
		if((y>kbd[16].starty)&&(y<kbd[16].endy)) {
		 for(i=16;i<27;i++)
			if((x>kbd[i].startx)
					&&(x<kbd[i].endx))
				break;
		 if(i==27) index=0xffff;else index=i;
		 if(index==16) { buf[0]=0;count=0;textout(0,144,"       ");
		  FindCandidate(buf,&start,&end);continue;}
		} else
		if ((y>kbd[27].starty)&&(y<kbd[27].endy)) {
		 for(i=27;i<38;i++)
			if((x>kbd[i].startx)\
			   &&(x<kbd[i].endx))
				break;
		 if(i==38) index=0xffff;else index=i;
		}else
		if((y>kbd[38].starty)&&(y<kbd[38].endy))  {
	 	for(i=38;i<49;i++)
			if((x>kbd[i].startx)\
			   &&(x<kbd[i].endx))
				break;
		if(i==49) index=0xffff;
		else 
		if(i==40){
			if(Upper)	
			 //ShowBMP(L_KEYBOARD,0,0);
			 draw_xbm(0,0,xbm_l_keyboard_width,xbm_l_keyboard_height,xbm_l_keyboard_bits);
			else
			 //ShowBMP(KEYBOARD,0,0);
			 draw_xbm(0,0,xbm_keyboard_width,xbm_keyboard_height,xbm_keyboard_bits);
			Upper=!Upper;
			index=0xffff;
			}
		 else 
			index=i;
		}
#ifdef DEBUGTRACE
		printf("handpad:before PinYin index=%d\n",index);
#endif
		if(index<49){	
		fillrect(kbd[index].startx,kbd[index].starty,
				kbd[index].endx+1,kbd[index].endy+1);
		if(Upper) buf[count]=toupper(kbd[index].ch);
		 else 
		   buf[count]=kbd[index].ch;
		buf[++count]=0;
		textout(0,144,buf);
		//printf("handpad:buf=%s,start=%d,end=%d\n",buf,start,end);
		
		FindCandidate(buf,&start,&end);
		//printf("handpad: After FindCandidate,start=%d,end=%d\n",start,end);
		if(start==end && start==0){
			buf[0]=0;
			textout(0,144,"           ");
			continue;
		}
		fseek(fp,start,SEEK_SET);
		if(end-start>18)
			{	
			fread(can,18,1,fp);
			can[18]=0;
			}
		else
			{
			fread(can,end-start,1,fp);	 
			can[end-start]=0;
			}
		#ifdef DEBUGTRACE
			printf("count:%d buf:%s can: %s\n",count,buf,can);	
		#endif

		//textout(0,144,"        ");
		//textout(0,144,buf);
		textout(0,144,"        ");
		textout(0,144,buf);
		textout(0,128,can);
		//printf("%d->%d\n",start,end);
		if(Upper)
		{
			for(i=0;i<10;i++)
			        buf[i]=0;
		}
	 	count=strlen(buf);	
	 	}
	}	
    else
    {
	static unsigned short oldx,oldy,err;    
	if(y>60&&y<127)
		{
		   err=(oldx>x)? oldx-x:x-oldx;
		   err+=(oldy>y)? oldy-y:y-oldy;
		   //err=!(*(volatile unsigned short*)(0xfffff418)&0x0040);
		   if(err>0&&err<20){
			   lineto(oldx=x,oldy=y);
		   }else{
			   moveto(oldx=x,oldy=y);
		   	}
		   
		}	
	index=0xffff;
    }
     }
   }    
}
