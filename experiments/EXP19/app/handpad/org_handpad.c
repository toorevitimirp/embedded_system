
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
#define KEYBOARD "/font/keyboard.bmp"
#define L_KEYBOARD "/font/l_keyboard.bmp"

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

void init_handpad()
{  
	*(volatile unsigned short*)(0xfffff304)|=0x0004;	
	*(volatile unsigned short*)(0xfffff42a)=0x0464;
	*(volatile unsigned short*)(0xfffff422)=0xff00;
	*(volatile unsigned short*)(0xfffff428)=0x659e;
	*(volatile unsigned short*)(0xfffff432)=0x211c;
	*(volatile unsigned short*)(0xfffff430)=0x1c3a;
	*(volatile unsigned char*)(0xfffff41b)|= 0x40;
	*(volatile unsigned char*)(0xfffff418)&= 0xbf;
}

int get_handpad(unsigned short *x,unsigned short *y)
{
	static unsigned char buff[4];
	char	flag=0;
		if(!(*(volatile unsigned short*)(0xfffff418)&0x0040)){
  			*(volatile unsigned short*)(0xfffff802)&=0xfd7f; //Clear IRQ

		  	*(volatile unsigned short*)(0xfffff430)=0x1c2a; 
 		 	*(volatile unsigned short*)(0xfffff428)=0x659e;  
		 	*(volatile unsigned short*)(0xfffff802)=0x423c;  
			*(volatile unsigned short*)(0xfffff428)=0x659a;  
  
		  	*(volatile unsigned short*)(0xfffff800)=0x1800;  
  
 		 	*(volatile unsigned short*)(0xfffff802)|=0x0100; 
 			
		  	while (!((*(volatile unsigned short*)(0xfffff802))&0x0080));
			buff[0]=(*(volatile unsigned short*)(0xfffff800));
 
		  	*(volatile unsigned short*)(0xfffff802)&=0xfd7f; //Clear IRQ
			*(volatile unsigned short*)(0xfffff428)=0x659e;  
		  	*(volatile unsigned short*)(0xfffff802)=0x423c;  
		  	*(volatile unsigned short*)(0xfffff430)=0x1c36;
		  	*(volatile unsigned short*)(0xfffff428)=0x659a; 
  

		  	*(volatile unsigned short*)(0xfffff800)=0x1C00;  
  
		  	*(volatile unsigned short*)(0xfffff802)|=0x0100; 
  
		  	while (!((*(volatile unsigned short*)(0xfffff802))&0x0080));
			buff[1] = *(volatile unsigned short*)(0xfffff800);
			
		  	*(volatile unsigned short*)(0xfffff802)&=0xfd7f; //Clear IRQ
			*(volatile unsigned short*)(0xfffff430)=0x1c3a;
			if(buff[0] == buff[2] && buff[1] == buff[3])
			{
				*x=buff[0]-HXMIN;
				*y=HYMAX-buff[1];
				if(*x>12) *x-=12;else *x=0;
				*x=((*x)*160)/202;
				if(*y<0) *y=0;
				#ifdef DEBUGTRACE
				printf("buf0=%d,buf1=%d,x=%d,y=%d\n",buff[0],buff[1],x,y);
				#endif
		 		flag=1;	
			}
			buff[2] = buff[0];
			buff[3] = buff[1];
		}
	if(flag) return 1;
	return 0;	
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

  if(!fp) return 0;

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
	//printf("Current Buf:%s %d",buf,len);
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
				if(GetItemText(t)==buf[i]) current=t;
				 else
					{
					 for(;t;t=GetNextSiblingItem(t))
					 {
						 if(GetItemText(t)==buf[i]) {	current=t;break;}
					 }

					 if(!t)
					 {
					 buf[i]=0;
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
				if(GetItemText(t)==buf[olen]) current=t;
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

/*main()
{
  short i;
  FILE *fp=fopen("gb.txt","rb");
  char buf[512];
  short start,end;
  Initialize();
  while(1)
  {
    printf("Input:");
    scanf("%s",buf);
    if(!strcmp("exit",buf)) break;
    FindCandidate(buf,&start,&end);
    if(end>start)
    {
       fseek(fp,start,SEEK_SET);
       if(end-start<512)
	 {
	 fread(buf,end-start,1,fp);
	 buf[end-start]=0;
	 }
	else
	 {
	 fread(buf,510,1,fp);
	 buf[510]=0;
	 }
	printf("%s\n",buf);
    }
    FindCandidate("",0,0);
  }
  Delete();
  return 0;
}*/

void main(void)
{
	unsigned short  i,x,y,index=0xffff;
	char buf[10],can[20];
	short Upper=1,count=0,start,end;
	FILE *fp=fopen("/font/gb.txt","rb");
			
	init_handpad();
 	initgraph();
	clearscreen();
	ShowBMP(KEYBOARD,0,0);
	setmode(MODE_SRC_XOR_DST);
	rectangle(0,60,159,127);
	if(!Initialize()) return;
	while(1){
	    
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
			if((x>kbd[i].startx)\
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
			 ShowBMP(L_KEYBOARD,0,0);
			else
			 ShowBMP(KEYBOARD,0,0);
			Upper=!Upper;
			index=0xffff;
			}
		 else 
			index=i;
		}
	//	printf("index=%d\n",index);
		if(index<49){	
		fillrect(kbd[index].startx,kbd[index].starty,
				kbd[index].endx+1,kbd[index].endy+1);
		if(Upper) buf[count]=toupper(kbd[index].ch);
		 else 
		   buf[count]=kbd[index].ch;
		buf[++count]=0;
		textout(0,144,buf);
		FindCandidate(buf,&start,&end);
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
