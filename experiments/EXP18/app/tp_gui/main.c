#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include<sys/types.h>
#include<sys/stat.h>
#include <stdlib.h>

//#include "../gui/gui.h"
#include "gui.h"
#include "handpad.h"
#include "./trans/ethernet.c"

#define STARTX 0
#define STARTY 0
#define Max 270000
#define WELCOME "welcome.bmp"

struct _keyboard{
        char ch;
        short startx,starty,endx,endy;
 	}
 kbdenter[]={
	 {'1',190+STARTX,114+STARTY,217+STARTX,128+STARTY}
	},
 kbdconfirm[]={
	 {'1',50+STARTX,130+STARTY,110+STARTX,148+STARTY},
         {'2',115+STARTX,130+STARTY,170+STARTX,148+STARTY}
	},
 kbdindex[]={
         {'1',41+STARTX,80+STARTY,135+STARTX,105+STARTY},
         {'2',41+STARTX,108+STARTY,137+STARTX,130+STARTY},
	 {'3',41+STARTX,139+STARTY,164+STARTX,165+STARTY},
         {'4',150+STARTX,219+STARTY,226+STARTX,239+STARTY},
	 {'5',53+STARTX,179+STARTY,136+STARTX,198+STARTY},
        },
 kbdtrans[]={
         {'1',160+STARTX,86+STARTY,205+STARTX,110+STARTY},
         {'2',160+STARTX,125+STARTY,201+STARTX,148+STARTY},
	 {'3',162+STARTX,180+STARTY,237+STARTX,199+STARTY},

        },
 kbdtree[]={
         {'1',25+STARTX,53+STARTY,45+STARTX,70+STARTY}
        },
 kbdexam[]={
         {'1',79+STARTX,160+STARTY,168+STARTX,191+STARTY}
        },
 kbdfontdemo[]={
         {'1',190+STARTX,280+STARTY,235+STARTX,295+STARTY}
        }

;


void delay()
{int i,j;
 for(i=0;i<4500;i++)
  for(j=0;j<2000;j++) {}
 }

void delay2()
{int i,j;
 for(i=0;i<10000;i++)
  for(j=0;j<3000;j++) {}
 }

void display()
	{textout(30,20,"Now transport the File",0x07e0,0x1111);
         rectangle(40,40,190,90);
         textout(30,100,"Now having finished    %",0xf800,0x3333);
         rectangle(50,130,110,148);
         rectangle(115,130,170,148);
         textout(51,131,"confirm",0xffff,0x0000);   //----------kbd2
         textout(116,131,"cancel",0xffff,0x0000);
	}

main()
{unsigned short  x,y,i,index0=0xffff,index10=0xffff,index11=0xffff,index12=0xffff,index3=0xffff,index4=0xffff;
 int flag=1,flag1=1,flag2=1,flag3=1,fileleng,pid=-1;
 char *bmpbuf,displaybuf[400],processbuf[30];
 FILE *ft;
 struct stat s;
/* displaybuf=(char*)malloc(400);
 if(ft=fopen("2.txt","r")==NULL)
	{printf("Can't find the 2.txt");
	 return 1;
	}
 stat("2.txt",&s);
 fileleng=s.st_size;
 printf("file size :%d\n",fileleng);
// displaybuf=ft;
// bzero(displaybuf,fileleng);
 fread(displaybuf,1,fileleng,ft);
// for(i=0;i<fileleng;i++)
//	{displaybuf[i]=*(ft+i);}
displaybuf[fileleng]='\0';
 printf("%s\n",displaybuf);*/
 initgraph();
 clearscreen();
 ShowBMP(WELCOME,0,0);
 init_handpad();
 setmode(MODE_SRC_XOR_DST);
// rectangle(172,110,237,127);
// textout(172,112,"---Enter",0x9999,0x1111);
 while(1)
  {if(get_handpad(&x,&y))
	{if((y>kbdenter[0].starty)&&(y<kbdenter[0].endy)&&(x>kbdenter[0].startx)&&(x<kbdenter[0].endx))
	  {//clearscreen();
	   break;
	  }
	}
  }
s1:
 index0=0xffff;
 index10=0xffff;
 index11=0xffff;
 index12=0xffff;
 index3=0xffff;
 index4=0xffff;
 
 clearscreen();
 ShowBMP("index.bmp",0,0);
 while(1)                       ///////////////////----1
       {if(get_handpad(&x,&y))
        printf("%d,%d\n",x,y);
            for(i=0;i<5;i++)
             {if((y>kbdindex[i].starty)&&(y<kbdindex[i].endy)&&(x>kbdindex[i].startx)&&(x<kbdindex[i].endx))
             break;}
	     if(i==5) index0=0xffff;
	     else 
	     {index0=i;
	      printf("%d\n",i);}
	   // else index0=0xffff;
        if(index0==0)
                {//clearscreen();                ///////////////////---2
                 ShowBMP("trans.bmp",0,0);
///////////////display  the file  trans/////////////////////////////////////////

		 while(1)                       //////////////////-----3
		       {if(get_handpad(&x,&y))
        		printf("%d,%d\n",x,y);
			for(i=0;i<3;i++)
			if((y>kbdtrans[i].starty)&&(y<kbdtrans[i].endy)&&(x>kbdtrans[i].startx)&&(x<kbdtrans[i].endx))
		          break;
			 if(i==3) index10==0xffff;
		         else
			   {index10=i;}
	//		else index10=0xffff;
			     // else index10=0xffff;
                    /////////trans the bmp files/////////////////////
        		if(index10==0)       ////////////////--------4
                		{clearscreen();
				 display();
				 bmpbuf=(char*)malloc(Max);
				 recvfile(bmpbuf,"look2.bmp");
				 //delay();
				 while(flag1)     /////////////--------5
				       {if(get_handpad(&x,&y))
        				printf("%d,%d\n",x,y);
					for(i=0;i<2;i++)
 		                         if((y>kbdconfirm[i].starty)&&(y<kbdconfirm[i].endy)&&(x>kbdconfirm[i].startx)&&(x<kbdconfirm[i].endx))
                             		  break;
					if(i==2) index11=0xffff;
					else 
					   {index11=i;}
	//				 else index11=0xffff;
        				if(index11==0)
                			 {clearscreen();
                 			  ShowBuf(bmpbuf,0,0);
					  free(bmpbuf);
					  delay2();
					  goto s1;
                 			  //flag1=0;
                			 }
				 	if(index11==1)
                			{clearscreen();
                 			 textout(20,20,"Trans files Failing",0xffff,0x1111);
                 			 //flag1=0;
					 delay2();
					 goto s1;
                			}
        			     }///////////////////////---------emd5
				  
                 		// ShowBuf(bmpbuf,0,0);
                 		 flag=0;       
                		}              /////// -------------end4
		    /////////////end the bmp files  trans////////////////
		   ////////////start the txt  files trans//////////////
        		if(index10==1)         ////////////////-------6
                		{clearscreen();
				 display();
				 bmpbuf=(char*)malloc(Max);
                                 recvfile(bmpbuf,"1.txt");
		//		 printf("bmpbuf==%s\n",bmpbuf);
			 	 while(1)         //////////////-----7
                                       {if(get_handpad(&x,&y))
                                        printf("%d,%d\n",x,y);
					for(i=0;i<2;i++)
                                         if((y>kbdconfirm[i].starty)&&(y<kbdconfirm[i].endy)&&(x>kbdconfirm[i].startx)&&(x<kbdconfirm[i].endx))
                                          break;
					if(i==2)  index12=0xffff;
					else		
					  {index12=i;}
	//				 else index12=0xffff;
                                        if(index12==0)
                                        {clearscreen();
                                         textout(0,0,bmpbuf,0xffff,0x1111);
					 free(bmpbuf);
					 delay2();
                                        // flag2=0;
					 goto s1;
                                        }
					if(index12==1)
                                        {clearscreen();
                                         textout(20,20,"Trans files Failing",0xffff,0x1111);
                                     //    flag2=0;
					 delay2();
					 goto s1;
                                        }
                                     }           //////////////////----end7
                                // ShowBuf(bmpbuf,0,0);
                                 flag=0;
                                }              ///////////////////--------end6
			 if(index10==2)
				{clearscreen();
				 goto s1;
				}
		     }			       //////////////////--------end3
		}	 			////////////////-------end2
	      ///////////////end the txt files trans//////////////////
///////////////end the file trans///////////////////////////

/////////////display the flash////////////////////////////
	if(index0==1)
                {//printf("sdlfsdflsdflsadlf\n");
		 clearscreen();
		 ShowBMP("1.bmp",0,0);
		 delay();
		 ShowBMP("2.bmp",0,0);
		 delay();
		 ShowBMP("1.bmp",0,0);
		 delay();
		 ShowBMP("3.bmp",0,0);
		 delay();
		 ShowBMP("1.bmp",0,0);
		 delay();
		 ShowBMP("2.bmp",0,0);
		 delay();
		 ShowBMP("1.bmp",0,0);
		 delay();
		 goto s1;
//                 break;
                }
/////////////end the flash///////////////////////////////////

/////////////display the book//////////////////////////////////
	if(index0==2)
		{clearscreen();
		 ShowBMP("shu.bmp",0,0);
		  while(1)
 			 {if(get_handpad(&x,&y))
        			if((y>kbdtree[0].starty)&&(y<kbdtree[0].endy)&&(x>kbdtree[0].startx)&&(x<kbdtree[0].endx))
          				{ShowBMP("shu2.bmp",0,0);
					 while(1)
						{if(get_handpad(&x,&y))
						 for(i=0;i<1;i++)
						 if((y>kbdexam[i].starty)&&(y<kbdexam[i].endy)&&(x>kbdexam[i].startx)&&(x<kbdexam[i].endx))
						   break;
						 if(i==1)   index3=0xffff;
						 else  index3=i;
			                         if(index3==0)
						    {clearscreen();
						     textout(0,0,"CC=m68k-pic-coff-gcc\nCOFF2FLAT=coff2flt\n\nCFLAGS=-I/ARM9.0",0xffff,0x1111);
		//			 	     displaybuf=(char*)malloc(400);
/* 	if(ft=fopen("2.txt","r")==NULL)
        {printf("Can't find the 2.txt");
         return 1;
        }
 	stat("2.txt",&s);
 	fileleng=s.st_size;
 	printf("file size :%d\n",fileleng);
// displaybuf=ft;
// bzero(displaybuf,fileleng);
 	fread(displaybuf,1,fileleng,ft);
//  memcpy(display,ft,fileleng);
// for(i=0;i<fileleng;i++)
//      {displaybuf[i]=*(ft+i);}
	displaybuf[fileleng]='\0';
// 	printf("%s\n",displaybuf);

						    textout(0,0,"displaybuf",0xffff,0x1111);*/}
						  //  free(displaybuf);
							break;}
					delay2();
					goto s1;
           				 //flag3=0;
          				}
        		}
		}
////////////////////////end book-----------------
//////////////start exit///////////////
       	if(index0==3)
		{clearscreen();
		 ShowBMP("exit.bmp",0,0);
		 delay2();
		 clearscreen();
		 break;
		} 
/////////////exit exit///////////////////
//////////////start fontdemo//////////////////
	if(index0==4)
		{ clearscreen();
		 // sprintf(processbuf,"./bin/fontdemo");
		  if((pid=vfork())==0)
		  {printf("pid=%d\n",pid);
		   execv("./mdemo",NULL);
//		   execl("/bin/fontdemo","fontdemo",NULL);
//		   execl("/mdemo","mdemo",NULL);
//		   delay2();
		  // kill(pid,0);
//		   clearscreen();
		   //goto s1;
//		   index0=0xffff;
//		   return;
		  }
		 else
		  {sleep(1);}
		 // kill(pid,0);
		  while(1)
			{//if(get_handpad(&x,&y)
			 rectangle(190,280,225,295);
			 textout(191,281,"exit",0xffff,0x1111);
			 kill(pid,0);
			 if(get_handpad(&x,&y))
                                for(i=0;i<1;i++)
				 if((y>kbdfontdemo[i].starty)&&(y<kbdfontdemo[i].endy)&&(x>kbdfontdemo[i].startx)&&(x<kbdfontdemo[i].endx))
				 break;
				 if(i==1)  index4=0xffff;
				 else
				 index4=i;
				 if(index4==0)
			//	  kill(pid,0);
			//	 ShowBMP("index.bmp",0,0);
				 goto s1;  
			//	 break;
			}
 		  }
/////////////////end fontdemo////////////////////
     }          ///////////////////////////--------1///////////////////
}///////////////////////////----end main
///////////////end the book/////////////////////////////////
// fillrectangle(20,20,100,100);
// textout(30,20,"Now transport the File",0x9999,0x1111);
//rectangle(40,40,190,90);
// textout(30,100,"Now having finished    %",0xeeee,0x3333);
// rectangle(50,130,110,148);
// rectangle(115,130,170,148);
// textout(51,131,"confirm",0xffff,0x0000);   //----------kbd2
// textout(116,131,"cancel",0xffff,0x0000);
// recvfile(bmpbuf);
/* while(1)
       {if(get_handpad(&x,&y))
       	printf("%d,%d\n",x,y);
          {if((y>kbd2[0].starty)&&(y<kbd2[0].endy))
	    {for(i=0;i<2;i++)
		if((x>kbd2[i].startx)&&(x<kbd2[i].endx))
             break;
	     if(i==2) index=0xffff;
	     else {index=i;}
           }
	   else index=0xffff;
        if(index==0)
		{clearscreen();
		 ShowBuf(bmpbuf,0,0);
		 break;
		}
	if(index==1)
		{clearscreen();
		 textout(20,20,"Trans files Failing",0x9999,0x1111);
		 break;
		}
	}
}*/

